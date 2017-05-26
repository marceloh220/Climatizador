/**************************************************************************************************************************

  Autor: Marcelo Henrique Moraes
  E-mail: marceloh220@hotmail.com
  Data: 5, abr. 2017
  Ultima Revisao: 15, mai. 2017
  Revisao Atual: 20, mai. 2017
  Copyright (c) 2017 Marcelo Henrique Moraes

  Projeto Casa Sustentavel: Climatizador de AR
  https://github.com/marceloh220/Climatizador.git

  Projeto core Marcelino
  https://github.com/marceloh220/Marcelino.git

  MCU: Atmega328p
  Arduino IDE 1.8.2
  Marcelino Atmega328/p (Arduino/Genuino Uno/Nano)
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
                                                  Definicoes de Hardware
***************************************************************************************************************************/

#include "hardware.h"

/**************************************************************************************************************************
                                                      Bibliotecas
***************************************************************************************************************************/

//Core Marcelino, compatibilidade com placas Arduino/Genuino UNO/Nano, Atmega328/p, Atmega168A/PA
#include <marcelino.h>

//Display lcd 16x2 com CI PCF8574 (TWI)
#include <IHM8574.h>

//Implementacao das classes separadas em arquivos
#include "classes/teclado.h"
#include "classes/controle.h"
#include "classes/temperatura.h"
#include "classes/relogio.h"
#include "classes/passo.h"

/*
   #include "classes/encoder.h"
   Implementei esta aplicacao para controle do encoder,
   mas estava tendo problemas durante a leitura por causa do overhead.
   Entao coloquei a aplicaca direramente na interrupcao do timer2
   que controla o motor de passo com leitura direta dos ports do motor de passo.
*/

/**************************************************************************************************************************
                                               Prototipo de funcoes auxiliares
***************************************************************************************************************************/

//Funcoes mostra informacoes no display
void mostraTemperatura();
void mostraVelocidade();
void mostraNivel();
void mostraHora();
void medirVolume();

//funcoes de controle de atuadores e do sistema
void acionamentos();
void desligamento();
void fechamento();

//interrupcoes do sistema
void motorPasso();              //interrupcao de overflow do timer 2
void resetWDT();                //interrupcao do WDT
void desligamentoProgramado();  //interrupcao int0 anexada quando o sistema liga
void ligamentoProgramado();     //interrupcao int0 anexada quando o sistema entra em modo sleep POWER DOWN

//funcao com erros previsiveis e imprevisiveis, apresenta no display alguma informacao que ajuda a solucionar o problema
void erro(uint8_t tipo);
#define erroHorizontal    1     //Problema na chave fim de curso
#define erroSensorNivel   2     //Problema no sensor de nivel do reservatorio
#define erroWDT           3     //O WDT reinicia a CPU se esta travar
#define erroMotorPasso    4     //Algum problema com o motor de passo
#define erroDesconhecido  NULL  //Algum problema que nao foi previsto

//Mede a quatidade de elementos dos vetores
#define tamVet(vet) (sizeof(vet)/sizeof((vet)[0]))

// === Vetor com as funcoes "mostraAlgumacoisa" ===

//Tipo para vetor de funcoes
typedef void (*funcoes)();

//ponteiro para funcoes
uint8_t mostraPTR = 0;  //As informacoes apresentadas no display serao manipuladas com este ponteiro

//vetor com funcoes,            ponteiro = 0,      ponteiro = 1,  ponteiro = 2, ponteiro = 3
funcoes volatile mostra[4] = {mostraTemperatura, mostraVelocidade, mostraNivel, mostraHora};

/**************************************************************************************************************************
                                            Constantes na memoria de programa
***************************************************************************************************************************/

//Le vetor na memoria de programa
#define get_pgm(m,p)   pgm_read_byte(&m[p])

//Caracter simbolo de graus para display lcd
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
  uint32_t ms2, ms10, ms60, ms500, s1, s30, m1, m5;
  uint32_t testes;//para testes gerais com temporizacoes
} timer_t;
timer_t temporizacao;

//Para medir volume de agua no reservatorio
class Volume {
  public:
    float     milimetros, centimetros, metros;
    float     litros, centilitros, mililitros;
    uint8_t   testes;
};
Volume reservatorio;

//Utiliza um endereco de memoria para realizar ate 8 testes booleanos atravez de flags
Register teste;
#define automatic  0    //flag que indica a movimentacao das paletas horizontais em movimento automatico
#define sinaliza   1    //flag que indica que deve sinalizar a movimentacao das paletas horizontais automatico
#define horizontal 2    //flag que indica que houve um erro nas paletas horizontais e o fechamento total nao foi possivel
#define manutencao 3    //flag que indica que o sistema esta em modo "manutencao"
#define acteclado  4    //flag que indica que ocorreu alguma acao no teclado
#define progOFF    5    //flag que indica que um desligamento programado foi requisitado
#define travaEnco  7    //flag que indica que o botao do encoder foi travado, apos o botao do encoder ser pressionado ele precisa ser solto para nova leitura

//Para leitura do encoder e controle do motor de passo da movimentacao das paletas horizontais
class Encoder {
  private:
    //vetor com estados para leitura do encoder em modo de quadratura
    int8_t estados[16] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };
    int16_t posicaoEncoder;
    int16_t posicaoPasso;
    uint16_t escala;
  public:
    //construtor
    Encoder(uint16_t esc = 1) {
      escala = esc;
    }
    //manipulacao dos atributos
    inline void encoder(int16_t pos) {
      posicaoEncoder += estados[pos];
    }
    inline void atualiza() {
      posicaoPasso = (posicaoEncoder >> 2) * escala;
    }
    inline int16_t passo() {
      return posicaoPasso;
    }
    inline int16_t posicao() {
      return posicaoPasso;
    }
    inline int16_t posicao(int16_t pos) {
      posicaoPasso = pos;
      posicaoEncoder = (pos / 5) | (posicaoEncoder & 0x03);
    }
    //teste de condicoes
    inline uint8_t limite(int16_t min, int16_t max) {
      return ( posicaoPasso >= min && posicaoPasso <= max );
    }
};
Encoder encoder(encoderEscala);

/**************************************************************************************************************************
                                      Inicializacao dos modulos do core Marcelino
***************************************************************************************************************************/

//manipulacao dos pinos digitais
Digital digital;  //Controle dos pinos digital

//temporizacoes diversas
Timer0  timer;  //Temporizacoes com o timer 0
Timer2  motor;  //Timer 2 para temporizar o acionamento do motor de passo

//leitura do sensor de nivel do reservatorio
Pulse   pulse;  //Modulo de medicao de largura de pulso para leiruta do sensor de nivel do reservatorio (pulseIn do Arduino)
Delay   delay;  //Um pequeno delay para o dispositivo HC-SR04 (sensor ultrassonico)

//apresentacao de dados para operacao, manutencao e debug de codigo
Serial  serial; //Modulo Serial para comunicacao com computador

//vigia da CPU, caso ela trave desliga os atuadores e reinicia o sistema para evitar problemas
WDT     wdt;  //Watch Dog Timer (WDT) para vigiar o MCU e evitar travamentos

//modo standby do sistema para economia de energia quando ocioso
Sleep   sleep;      //Coloca o controlador em modo de baixo consumo
External external;  //Interrupcao externa para acionamento e desacionamento no modo sleep

/**************************************************************************************************************************
                                              Instancias de Objetos
***************************************************************************************************************************/

IHM8574 display(displayADDRESS);                        //Display 16x2 com ci PCF8574
Relogio relogio(pt_br);                                 //Relogio RTC com dispositivo DS3231, semana e mes em pt_br
Temperatura temperatura(pinLM35, 30);                   //Temperaturas com sensor de temperatura do DS3231 e LM35, media de 30 leituras analogicas
Controle controle(relayADDRESS, INVERSO);               //Controle dos atuadores com logica inversa (dreno de corrente)
Teclado teclado(pinTeclado);                            //Leitura do teclado analogico
Passo passo(motorPA, motorPB, motorPC, motorPD, ANODO); //Motor de passo da movimentacao das paletas horizontais em modo de ANODO comum

/**************************************************************************************************************************
                                                   Funcoes principais
***************************************************************************************************************************/


//Funcao de configuracao do MCU
void setup() {

  //Liga o background do display, talves algum dia mude o nome de background para backlight...
  display.background(ON);

  //escreve uma apresentacao
  display.print("  Climatizador  ");
  serial.println("  Climatizador  ");
  display.set(0, 1);
  display.print(" ENG. Eletrica  ");
  serial.println(" ENG. Eletrica  ");

  //Salva caracter do simbolo de graus celcius na posicao 0 da memoria grafica do display
  //display.create(posicao da memoria grafica, linha do simbolo, interador para salvar as oito linhas da matriz)
  for (int i = 0; i < 8; i++)
    display.create(0, get_pgm(graus, i), i);

  //configura os pinos do reles
  controle.configura(velocidade1, velocidade2, velocidade3, bombaDagua, direcaoVertical, livre1, livre2, pinSinalizacao);

  //Inicia com todos os reles desligado
  controle.parada();

  //coloca os pinos de alguns hardware como entradas com resistor de pullup interno do MCU ativado
  digital.pullup(5, encoderA, encoderB, encoderButton, pinfimdeCurso, pinPOWER);

  //Inicia com a ventilacao fechada
  fechamento();

  motor.period(tempoUsMotorPasso);  //configura o timer utilizado para controlar o motor de passo para gerar a interrupcao no tempo programado (us)
  motor.attach(OVF, motorPasso);    //anexa na interrupcao de overflow (OVF) do temporizador uma funcao que ira conter o algoritimo que ira controlar o motor de passo

  wdt.config(INT_RESET);  //configura o watch dog timer (WDT) para resetar o MCU se ele travar e executa uma interrupcao antes de resetar
  wdt.timeout(W_8S);      //configura o estouro do WDT para 8 segundos
  wdt.attach(resetWDT);   //anexa a funcao "resetWDT()"  na interrupcao do WDT
  wdt.enable();           //habilita o WDT

  external.attach(INT0, FALLING, desligamentoProgramado);

  delay.ms(2000); //aguarda um tempo para a apresentacao
  medirVolume();  //Atualiza a leitura de volume do reservatorio

}//fim da funcao setup


//Funcao para execucao do codigo em ciclo infinito.
void loop() {

  /*
    Usei isso como debug para determinar os valores
    maximos e minimos para o encoder e o motor de passo

    serial.print("Encoder passo: ");
    serial.println(encoder.posicao());
  */

  //Tarefa realizada a cada 10 milisegundo
  if ( (timer.millis() - temporizacao.ms10) >= 10) {  //Testa se passou 10ms

    temperatura.atualiza();                           //Atualiza as leituras de temperatura
    acionamentos();                                   //Chama funcao de acoes de controle
    mostra[mostraPTR]();                              //Chama funcao alocada na posicao do ponteiro mostra

    temporizacao.ms10 = timer.millis();               //Salva o tempo atual para nova tarefa apos 10ms

  }//fim da tarefa de 10ms

  //Tarefa realizada a cada 60 milisegundo
  if ( (timer.millis() - temporizacao.ms60) >= 60) {  //Testa se passou 60ms

    teclado.liberar();                                //Libera o teclado para nova leitura, o tempo de 60ms garante o debounce das teclado

    if (digital.ifset(encoderButton))  //Se o botao do encoder foi solto
      teste.clear(travaEnco);          //Destrava a leitura do botao

    temporizacao.ms60 = timer.millis();               //Salva o tempo atual para nova tarefa apos 60ms

  }//fim da tarefa de 60ms

  //Tarefa realizada a cada 500 milisegundo
  if ( (timer.millis() - temporizacao.ms500) >= 500) {  //Testa se passou 500ms

    //verifica se foi requerido entrada em modo de baixo consumo
    if (teste.ifset(progOFF))
      desligamento();

    relogio.sinalizar();  //Sinaliza ajuste do relogio com blink da configuracao selecionada

    //sinalizado de movimentacao automatica das paletas horizontais
    if (teste.ifset(sinaliza))        //Se sinalizacao ligada
      controle.reles(livre1, HIGH);   //Liga sinalizacao de automatico
    else controle.reles(livre1, LOW); //Se nao, desliga led de sinalizacao

    temporizacao.ms500 = timer.millis();  //Salva o tempo atual para nova tarefa apos 500ms

  }//fim da tarefa de 500ms

  //Tarefa realizada a cada 1 segundo
  if ( (timer.millis() - temporizacao.s1) >= 1000) {  //Testa se passou 1 segundo

    medirVolume();  //Atualiza a leitura de volume do reservatorio

    //verifica se a palheta horizontal esta aberta enquanto a ventilacao principal esta ligada
    //ligar a ventilacao sem que tenha uma abertura para passagem do vento aumenta a carga do motor
    if (!digital.read(pinfimdeCurso) && controle.velocidade() > 0) {  //se ventilacao foi ligada mas a apalheta nao abril

      controle.velocidade(0); //desliga a ventilacao principal
      passo.passos(0);        //zera a contagem de passos
      erro(erroMotorPasso);   //sinaliza o erro

    }//fim da verificacao de abertura da palheta horizontais

    controle.sinalizar(); //chama a sinalizacao para o nivel de agua

    temporizacao.s1 = timer.millis(); //Salva o tempo atual para nova tarefa apos 1s

  }//fim da tarefa de 1s

  //Tarefa realizada a cada 30 segundo
  if ( (timer.millis() - temporizacao.s30) >= 30000) {  //Testa se passou 30 segundo

    //posiciona o menu para mostrar temperatura
    relogio.posicao(0); //zera o posicionamento de configuracao do relogio
    mostraPTR = 0;      //posiciona ponteiro para mostrar temperaturas

    temporizacao.s30 = timer.millis();  //Salva o tempo atual para nova tarefa apos 30s

  }//fim da tarefa de 30s

  //Tarefa realizada a cada 1 minuto
  if ( (timer.millis() - temporizacao.m1) >= 60000) { //Testa se passou 1min

    display.background(OFF);  //desliga a luz de fundo do display

    temporizacao.s30 = timer.millis();  //Salva o tempo atual para nova tarefa apos 1min

  }//fim da tarefa de 1min

  //Tarefa realizada a cada 5min
  if ( (timer.millis() - temporizacao.m5) >= 300000) {  //Testa se passou 5min

    //se a ventilacao estiver desligada, desliga o sistema
    if (controle.velocidade() == 0)
      desligamento();

    temporizacao.m5 = timer.millis(); //Salva o tempo atual para nova tarefa apos 5min

  }//fim da tarefa de 5min

  //se aconteceu alguma acao no teclado ou sistema em modo manutencao
  if (teste.ifset(acteclado) || teste.ifset(manutencao)) {
    teste.clear(acteclado);
    display.background(ON);

    //impede que as tarefas de 30 segundos, 1 minuto e 5 minutos sejam realizadas
    temporizacao.s30 = timer.millis();  //impede a mudança do display
    temporizacao.m1 = timer.millis();   //impede o desligamento da luz de fundo do display
    temporizacao.m5 = timer.millis();   //impede que o sistema seja desligado mesmo com a ventilacao desligada

  }//fim dos testes de manutencao e acao de teclado detectado

  wdt.clear();  //Limpa o watch dog timer (WDT) para evitar reset

  if ( teste.ifset(horizontal) )  //se a flag de erro da movimentacao das palhetas horizontais estiver setada
    erro(erroHorizontal);         //apresenta no teclado que houve o erro

}//fim da funcao loop

/**************************************************************************************************************************
                                                Funcoes auxiliares
***************************************************************************************************************************/

//Implementacoes separadas em arquivos para faciliar a leitura e solucao de problemas

#include "mostraTemperatura.h"  //mostra temperaturas
#include "mostraVelocidade.h"   //mostra velocidades da ventilacao principal
#include "mostraNivel.h"        //mostra o nivel de agua do reservatorio
#include "mostraHora.h"         //mostra o horario

#include "acionamentos.h"       //controla os atuadores
#include "desligamento.h"       //desligamento programado do sistema
#include "fechamento.h"         //fecha as palhetas horizontais

#include "medirVolume.h"        //medicao do volume de agua do reservatorio

#include "interrupcoes.h"       //interrupcoes do microcontrolador

#include "erros.h"              //aviso de erros no sistema

