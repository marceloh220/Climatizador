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

//Arquivo com definicoes de hardware do sistema
#include "hardware.h"

#include <IHM8574.h>

//Classes separadas em arquivos
#include "teclado.h"
#include "ventilacao.h"
#include "temperatura.h"
#include "relogio.h"


/**************************************************************************************************************************
                                               Prototipo de funcoes auxiliares
***************************************************************************************************************************/

void mostraTemperatura();
void mostraVelocidade();
void mostraNivel();
void mostraHora();
void acao();
void medirVolume();

typedef void (*funcoes)();
funcoes mostra[] = {mostraTemperatura, mostraVelocidade, mostraNivel, mostraHora};
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

//Para temporizacoes
typedef struct Time {
  uint32_t ms10, ms60, ms500, s1;
} time_t;
time_t temporizacao;

//Para medir volume de agua no reservatorio
class Volume {
  public:
    float milimetros, centimetros, metros;
    float litros, centilitros, mililitros;
    uint8_t ovf;
    uint64_t captura;
};
Volume volume;


/**************************************************************************************************************************
                                      Inicializacao dos modulos do core Marcelino
***************************************************************************************************************************/

Digital digital;          //Controle dos pinos digital
Timer0  timer;            //Temporizacoes com o timer 0
Timer1  captura;          //Captura para leitura de largura de pulso do sensor de nivel com hardware do timer 1
Serial  serial(57600);    //Comunicacao Serial com um comptador
Delay   delay;            //Um pequeno delay para o dispositivo HC-SR04 (sensor ultrassonico)


/**************************************************************************************************************************
                                              Instancias de Objetos
***************************************************************************************************************************/

IHM8574 display(displayADDRESS);        //Display 16x2 com ci PCF8574
Relogio relogio(pt_br);                  //Relogio RTC com dispositivo DS3231
Temperatura temperatura(pinLM35, 30);   //Temperaturas com sensor de temperatura do DS3231 e LM35
Ventilacao ventilacao;                  //Controle da velocidade da ventilacao
Teclado teclado(pinTeclado);            //Leitura do teclado analogico

//Objeto instanciado no arquivo da classe ventilacao
//extern Reles rele(relayADDRESS, DIRETO);  //Controle dos reles com PCF8574 com logica direta


/**************************************************************************************************************************
                                                Funcoes em Interrupcoes
***************************************************************************************************************************/

//interrupcao de estouro do temporizador de captura (overflow)
void medeVolumeOVF()
{
  //Quando o timer de captura estoura, registra um overflow para somar ao tempo da captura
  volume.ovf++;
}//fim da interrupcao de overflow do temporizador de captura

//interrupcao de deteccao de sinal no pino de captura
void medeVolume()
{
  
  if ( captura.rising() ) {     //Se detectado a borda de subida

    captura.timer(CLEAR);       //Limpa o temporizador de captura
    captura.prescale(64);       //Liga o temporizador de captura com prescale 64

    //configura captura para detectar a borda de descida do sinal
    captura.attach(CAPT, FALLING, medeVolume);

  }//fim da deteccao da borda de subida

  else {                        //Se nao, quando detectado a borda de descida

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
        rele.troca(pinSinalizacao);                 //Sinaliza nivel de agua baixo no reservatorio com um led

      else                                          //Se ventilacao desligada
        rele.desliga(pinSinalizacao);               //Nao incomoda ninguem com sinalizacoes desnecessarias

    }//fim do teste de nivel de agua

    temporizacao.s1 = timer.millis();               //Salva o tempo atual para nova tarefa apos 1s

  }//fim da tarefa de 1s

}//fim da funcao loop

/**************************************************************************************************************************
                                                Funcoes auxiliares
***************************************************************************************************************************/

//Implementacoes separaas em arquivos para faciliar a leitura e solucao de problemas
#include "mostraTemperatura.h"
#include "mostraVelocidade.h"
#include "mostraNivel.h"
#include "mostraHora.h"
#include "acao.h"
#include "medirVolume.h"

