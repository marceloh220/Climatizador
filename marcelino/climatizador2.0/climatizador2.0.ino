/**************************************************************************************************************************

   Autor: Marcelo Henrique Moraes
   E-mail: marceloh220@hotmail.com
   Copyright (c) 2017 Marcelo Henrique Moraes

   Projeto Casa Sustentavel: Climatizador de AR
   https://github.com/marceloh220/Climatizador.git

   Codigo fonte e bibliotecas disponiveis em:
   https://github.com/marceloh220/Climatizador/tree/master/arduino

   Esquematicos do projeto em formato PDF (Apenas leitura) e DSN (simulação Proteus 7.7)
   https://github.com/marceloh220/Climatizador/tree/master/Schematics

   Projeto core Marcelino
   https://github.com/marceloh220/Marcelino.git

   Arduino IDE 1.8.2
   Marcelino Atmega328 (ATmega328/p)
   F_CPU 16MHz (Cristal externo)

    This application is a free software, you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.
    This application is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY, without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    https://www.gnu.org/licenses/gpl-3.0.en.html

***************************************************************************************************************************/



/**************************************************************************************************************************
                                                      Bibliotecas
***************************************************************************************************************************/
#include "hardware.h"

#include <IHM8574.h>
#include <DS3231.h>

#include "teclado.h"
#include "ventilacao.h"
#include "temperatura.h"

/**************************************************************************************************************************
                                      Inicializacao dos modulos do core Marcelino
***************************************************************************************************************************/

Digital digital;
Timer0  timer;
Timer1  captura;
TWI     twi;
Serial  serial(57600);
Delay   delay;

/**************************************************************************************************************************
                                               Prototipo de funcoes auxiliares
***************************************************************************************************************************/

void mostraTemperatura();
void mostraHumidade();
void mostraVelocidade();
void mostraNivel();
void mostraHora();
void acao();
void medirVolume();

typedef void (*funcoes)();
funcoes mostra[] = {mostraTemperatura, mostraHumidade, mostraVelocidade, mostraNivel, mostraHora};
uint8_t mostraPTR = 0;


/**************************************************************************************************************************
                                               Constantes do sistema
***************************************************************************************************************************/

#define get_pgm(m,p)   pgm_read_byte(&m[p])
const uint8_t graus[8] PROGMEM =
{
  0b00000110,
  0b00001001,
  0b00001001,
  0b00000110,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000
};


/**************************************************************************************************************************
                                                  Variaveis de sistema
***************************************************************************************************************************/

uint8_t configRelogio;

typedef struct Time {
  uint32_t ms10, ms60, ms500, s1;
} time_t;

time_t temporizacao;

class Volume {
  public:
    float milimetros, centimetros, metros;
    float litros, centilitros, mililitros;
    uint8_t ovf;
    uint64_t captura;
};

Volume volume;


/**************************************************************************************************************************
                                              Instancias de Objetos
***************************************************************************************************************************/

IHM8574 display(displayADDRESS);

DS3231 relogio(pt_br);

Temperatura temperatura(pinLM35, 30);

Ventilacao ventilacao;

Teclado teclado(pinTeclado);


/**************************************************************************************************************************
                                                Funcoes em Interrupcoes
***************************************************************************************************************************/

/*
   Os processadores executam as instrucoes de forma sequencial, uma apos outra,
   porem existem funcoes que podem ser chamadas para execulcao tao logo um evento ocorra.
   Essas funcoes sao as interrupcoes, elas interrompem a execucao do programa
   e saltam para atender a interrupcao gerada.

   Microcontroladores AVR possuem um vetor para cada fonte de interrupcao, no caso do Atmega328
   sao 25 fontes de interrupcao que vao desde interrupcoes externas ate a leitura da memoria de programa terminada.

   Com essas interrupcoes o desenvolvedor pode colocar um hardware do MCU para trabalhar sem constante intervencao da CPU
   e quando este hardware terminar sua tarefa ele gera um sinal de interrupcao para o processador atender.

   No modulo do timer 0 do core Marcelino existe uma interrupcao implicita que e execudata a cada 1ms,
   essa interrupcao e rapida porem util, pois gera uma constante de tempo que pode ser utilizada de
   diversas maneiras na aplicação.
   Essa constante de tempo pode ser visualizada atravez do metodo millis().

   Alem da interrupcao do timer 0 para geracao de constantes de tempo,
   esta aplicacao utiliza a interrupcao do timer 1 no modo captura para
   medir a largura de um pulso externo.

   Algumas aplicacoes encontradas na internet fazem essa medicao de largura de pulso
   por software, o que prende a CPU durante a leitura, o que gasta tempo da CPU
   alem de reduzir a precisao da medicao.
   O timer 1 do MCU Atmega328 possui um hardware capaz de medir a largura de um pulso externo
   sem a necessidade de gasto do tempo da CPU, uma vez esse hardware configurado
   o processador fica livre para realizar outras tarefas.
   Quando a medicao e finalizada e gerado um sinal de interrupcao
   entao a CPU pode ser requisitada para realizar os calculos.
*/

/*
   Quando o temporizador do hardware de captura atinge o seu valor maximo,
   ele ira gerar uma interrupcao que ira jogar o processador para atender esta funcao.
*/
void medeVolumeOVF()
{
  //interrupcao de estouro do temporizador de captura (overflow)

  //Quando o timer de captura estoura, registra um overflow para somar ao tempo da captura
  volume.ovf++;

}//fim da interrupcao de overflow do temporizador de captura

/*
   Quando uma borda de subida ou descida e detectado no pino de captura ICP (pino Digital 8),
   sera gerado uma interrupcao que ira jogar o processador para atender esta funcao.

   Como as mudancas configuradas sao enviadas para a mesma funcao
   e preciso testar se foi detectado a borda de subida ou descida do sinal.
*/
void medeVolume()
{
  //interrupcao de deteccao de sinal no pino de captura

  //Se detectado a borda de subida
  if ( captura.rising() ) {

    captura.timer(CLEAR);       //Limpa o temporizador de captura
    captura.prescale(64);       //Liga o temporizador de captura com prescale 64

    //configura captura para detectar a borda de descida do sinal
    captura.attach(CAPT, FALLING, medeVolume);

  }//fim da deteccao da borda de subida

  //Se nao, quando detectado a borda de descida
  else {

    captura.prescale(OFF);      //Desliga o temporizador da captura

    //Salva o tempo de captura mais os overflows registrados
    volume.captura = captura.capt() + volume.ovf * 65535;
    //O temporizador de captura e de 16bis, portanto cada overflow equivale a 65535 ciclos

    //Limpa os overflows
    volume.ovf = 0;

    //Desanexa as funcoes de captura e overflow das interrupcoes
    captura.detach(CAPT);       //Para a interrupcao de captura (CAPT)
    captura.detach(OVF);        //Para a interrupcao de overflow (OVF)

  }//fim da deteccao da borda de descida

}//fim da interrupcao de deteccao de sinal no pino de captura


/**************************************************************************************************************************
                                                   Funcoes principais
***************************************************************************************************************************/

//Funcao de configuracao do MCU
//Umas das primeiras funcoes executadas, e chamada apenas uma vez quando o MCU e iniciado ou reiniciado
void setup()
{

  //display.create(posicao da memoria grafica, linha do simbolo, interador para salvar as oito linhas da matriz)

  //Salva caracter do simbolo de graus celcius na posicao 0 da memoria grafica do display
  for (int i = 0; i < 8; i++)
    display.create(0, get_pgm(graus, i), i);

  //Liga o background do display
  display.background(LIGADO);

  //Pino do LED como saida
  digital.mode(pinSinalizacao, OUTPUT);

  //Inicia com todos os reles desligado
  for (int i = 0; i < 8; i++)
    rele.desliga(i);

}//fim da funcao setup

//Funcao para execucao do codigo em ciclo infinito.
//Fica em constante execucao enquanto o MCU estiver sendo alimentado.
void loop()
{

  //Tarefa realizada a cada 10 milisegundo
  if ( ( timer.millis() - temporizacao.ms10 ) >= 10) {    //Testa se passou 10ms

    temperatura.update();                   //Atualiza as leituras de temperatura
    acao();                                 //Chama funcao de acoes de controle
    mostra[mostraPTR]();                    //Chama funcao alocada na posicao do ponteiro mostrafuncao

    temporizacao.ms10 = timer.millis();     //Salva o tempo atual para nova tarefa apos 10ms

  }//fim da tarefa de 10ms

  //Tarefa realizada a cada 60 milisegundo
  if ( ( timer.millis() - temporizacao.ms60 ) >= 60) {    //Testa se passou 60ms

    teclado.liberar();                      //Libera o teclado para nova leitura, o tempo de 60ms garante o debounce das teclas

    temporizacao.ms60 = timer.millis();     //Salva o tempo atual para nova tarefa apos 60ms

  }//fim da tarefa de 60ms

  //Tarefa realizada a cada 500 milisegundo
  if ( ( timer.millis() - temporizacao.ms500 ) >= 500) {  //Testa se passou 500ms

    medirVolume();                          //Atualiza a leitura de volume do reservatorio

    temporizacao.ms500 = timer.millis();    //Salva o tempo atual para nova tarefa apos 500ms

  }//fim da tarefa de 500ms

  //Tarefa realizada a cada 1 segundo
  if ( ( timer.millis() - temporizacao.s1 ) >= 1000) {    //Testa se passou 1 segundo

    //Testa se o nivel de agua esta baixo
    if ( volume.mililitros < nivelMIN ) {

      if ( ventilacao.velocidade() > 0 )            //Se ventilacao esta ligada
        rele.troca(pinSinalizacao);                   //Sinaliza nivel de agua baixo no reservatorio com um led

      else                                          //Se ventilacao desligada
        rele.desliga(pinSinalizacao);                //Nao incomoda ninguem com sinalizacoes desnecessarias

    }//fim do teste de nivel de agua

    temporizacao.s1 = timer.millis();       //Salva o tempo atual para nova tarefa apos 1s

  }//fim da tarefa de 1s

}//fim da funcao loop


/**************************************************************************************************************************
                                                Funcoes auxiliares
***************************************************************************************************************************/

//Funcao que mostra no display a temperaturas
void mostraTemperatura()
{
  display.set(0, 0);                      //Posiciona cursor na coluna 0 / linha 0
  display.print("TempA: ");
  display.print(temperatura.ambiente());
  display.write((uint8_t)0);              //Mostra caracter simbolo graus
  display.print("C  ");
  display.set(0, 1);                      //Posiciona cursor na coluna 0 / linha 1
  display.print("TempI: ");
  display.print(temperatura.refrigerada());
  display.write((uint8_t)0);              //Mostra caracter simbolo graus
  display.print("C  ");
}//fim da funcao mostraTemperatura

//Funcao que mostra no display a humidade
void mostraHumidade() {
  display.set(0, 0);                      //Posiciona cursor na coluna 0 / linha 0
  display.print("Humidade       ");
  display.set(0, 1);                      //Posiciona cursor na coluna 0 / linha 1
  display.print("BlaBlaBla       ");      //O algoritimo do sensor de humidade ainda nao foi definidos
}//fim da funcao mostraHumidade

//Funcao que mostra no display a velocidade
void mostraVelocidade() {
  display.set(0, 0);                      //Posiciona cursor na coluna 0 / linha 0
  uint8_t vel = ventilacao.velocidade();  //Verifica a velocidade
  if (vel == 0) {
    display.print("Ventilacao      ");
    display.set(0, 1);
    display.print("Desligada       ");
  }
  else if (vel == 1) {
    display.print("Velocidade      ");
    display.set(0, 1);
    display.print("Baixa           ");
  }
  else if (vel == 2) {
    display.print("Velocidade      ");
    display.set(0, 1);
    display.print("Media           ");
  }
  else if (vel == 3) {
    display.print("Velocidade      ");
    display.set(0, 1);
    display.print("Alta            ");
  }
}//fim da funcao mostraVelocidade

//Funcao que mostra no display o nivel de agua
void mostraNivel() {
  display.set(0, 0);                      //Posiciona cursor na coluna 0 / linha 0
  display.print("Nivel de Agua     ");
  display.set(0, 1);                      //Posiciona cursor na coluna 0 / linha 1
  display.print(volume.mililitros);      //Mostra nivel de agua calculado
  display.print(" mL           ");
}//fim da funcao mostraNivel

//Funcao que mostra no display a hora
void mostraHora() {
  uint8_t aux;
  display.set(0, 0);                      //Posiciona cursor na coluna 0 / linha 0
  display.print("    ");
  aux = relogio.hour();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print(':');
  aux = relogio.minute();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print(':');
  aux = relogio.second();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print("    ");
  display.set(0, 1);                      //Posiciona cursor na coluna 0 / linha 1
  display.print(relogio.weekSTR());
  display.print(' ');
  aux = relogio.day();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print(' ');
  display.print(relogio.monthSTR());
  display.print(" 20");
  aux = relogio.year();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print(' ');
  if (configRelogio && mostraPTR == 4) {
    display.cursor(onCURSOR);
    if (configRelogio == 1 )
      display.set(5, 0);
    else if (configRelogio == 2 )
      display.set(8, 0);
    else if (configRelogio == 3 )
      display.set(11, 0);
    else if (configRelogio == 4 )
      display.set(2, 1);
    else if (configRelogio == 5 )
      display.set(5, 1);
    else if (configRelogio == 6 )
      display.set(9, 1);
    else if (configRelogio == 7 )
      display.set(14, 1);
  }
  else
    display.cursor(noCURSOR);
}//fim da funcao mostraHora

//Funcao que realiza a acao da tecla pressionada
void acao() {
  uint8_t tecla = teclado.leitura();  //Verifica a tecla pressionada
  if ( tecla == 1) {                  //Se pressionada tecla 1
    configRelogio = 0;
    if (mostraPTR > 0)                //Se ponteiro mostrar nao estiver na posicao 0
      mostraPTR--;                    //Decrementa ponteiro mostrar
  }
  else if ( tecla == 2) {             //Se pressionada tecla 2
    configRelogio = 0;
    if (mostraPTR < 4)                //Se ponteiro mostrar nao estiver na posicao 4
      mostraPTR++;                    //Incrementa ponteiro mostrar
  }
  else if ( tecla == 3) {               //Se tecla 3 pressionada
    configRelogio = 0;
    ventilacao.trocar();              //Troca a velocidade da ventilacao
  }
  else if ( tecla == 4) {             //Se tecla 4 pressionada
    configRelogio++;
    if (configRelogio > 7)
      configRelogio = 0;
  }
  else if ( tecla == 5) {

    if (configRelogio == 1) {
      tecla = relogio.hour();
      if (tecla > 0) {
        tecla--;
        relogio.hour(tecla);
      }
      else if (tecla == 0) {
        relogio.hour(23);
      }
    }
    if (configRelogio == 2) {
      tecla = relogio.minute();
      if (tecla > 0) {
        tecla--;
        relogio.minute(tecla);
      }
      else if (tecla == 0) {
        relogio.minute(59);
      }
    }
    if (configRelogio == 3) {
      tecla = relogio.second();
      if (tecla > 0) {
        tecla--;
        relogio.second(tecla);
      }
      else if (tecla == 0) {
        relogio.second(59);
      }
    }
    if (configRelogio == 4) {
      tecla = relogio.week();
      if (tecla > 1) {
        tecla--;
        relogio.week(tecla);
      }
      else if (tecla == 1) {
        relogio.week(7);
      }
    }
    if (configRelogio == 5) {
      tecla = relogio.day();
      if (tecla > 1) {
        tecla--;
        relogio.day(tecla);
      }
      else if (tecla == 1) {
        relogio.day(31);
      }
    }
    if (configRelogio == 6) {
      tecla = relogio.month();
      if (tecla > 1) {
        tecla--;
        relogio.month(tecla);
      }
      else if (tecla == 1) {
        relogio.month(12);
      }
    }
    if (configRelogio == 7) {
      tecla = relogio.year();
      if (tecla > 0) {
        tecla--;
        relogio.year(tecla);
      }
      else if (tecla == 0) {
        relogio.year(99);
      }
    }
  }
}//fim da funcao acao

//Funcao que inicia a medicao do volume de agua do reservatorio
void medirVolume() {
  digital.mode(pinUltrason, OUTPUT);                //pino de trigger do sensor de ultrasson configurado como saida
  captura.attach(OVF, medeVolumeOVF);               //anexa a funcao 'medeVolumeOVF' na interrupcao de overflow do temporizacao do hardware de captura
  captura.attach(CAPT, RISING, medeVolume);         //anexa a funcao 'medeVolume' na interrupcao de captura do MCU para detectar uma borda de subida do sinal no pino ICP
  digital.write(pinUltrason, LIGADO);               //liga o pulso de ultrassom do sensor HC-SR04
  delay.us(15);                                     //aguarda um tempo para que os pulsos ultrassonicos sejam enviados pelo sensor
  digital.write(pinUltrason, DESLIGADO);            //desliga o pulso de ultrassom do sensor
  //calcula as distancias do sensor ultrassonico
  // Distancia = Largura do Pulso * Velocidade do Som / 2
  // Largura do Pulso = 1/F_CPU * prescale * captura
  // Distancia = 1/F_CPU * prescale * captura * Velocidade do Som / 2
  // Distancia = captura * 1/16e6[s] * 64 * 340.29[m/s] / 2
  // Distancia = captura * 0.00068058 [m]
  volume.metros = volume.captura * 0.00068058;
  //cm = m * 10e2
  volume.centimetros = volume.captura * 0.068058;
  //mm = m * 10e3
  volume.milimetros = volume.captura * 0.68058;
  //subtrai a altura do reservatorio da distancia medida pelo sensor
  float aux = alturaReservatorio - volume.milimetros;
  //Evita valores negativos para no nivel de agua (Claro! Seria negativo porque, o reservatorio ta devendo agua pra alguem?)
  if (aux < 0)
    aux = 0;
  //Calcula o volume do reservatorio com base nas caracteristicas fisicas dele
  volume.mililitros = aux * comprimentoReservatorio * larguraReservatorio;
  volume.centilitros = volume.mililitros / 10.0;
  volume.litros = volume.mililitros / 1000.0;
  //Se o nivel de agua esta abaixo do predeterminado
  if ( volume.mililitros < nivelMIN )
    rele.desliga(bombaDagua);                      //desliga a bomba d'agua
  //Se nao, se ventilacao ligada
  else if ( ventilacao.velocidade()) {
    rele.desliga(pinSinalizacao);                  //desliga sinalizacao de nivel de agua baixo
    rele.liga(bombaDagua);                         //liga a bomba d'agua
  }
  else {
    rele.desliga(pinSinalizacao);                  //desliga sinalizacao de nivel de agua baixo
    rele.desliga(bombaDagua);                      //desliga a bomba d'agua
  }
}//fim da funcao medirVolume

