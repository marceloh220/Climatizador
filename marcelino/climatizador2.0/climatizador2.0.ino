/**************************************************************************************************************************

   Autor: Marcelo Henrique Moraes
   E-mail: marceloh220@hotmail.com
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

//Core Marcelino, compatibilidade com placas Arduino/Genuino UNO/Nano, Atmega328/p, Atmega168
#include <marcelino.h>

//Display lcd 16x2 com CI PCF8574 (TWI)
#include <IHM8574.h>

//Classes separadas em arquivos
#include "classes/teclado.h"
#include "classes/controle.h"
#include "classes/temperatura.h"
#include "classes/relogio.h"
#include "classes/passo.h"

/**************************************************************************************************************************
                                               Prototipo de funcoes auxiliares
***************************************************************************************************************************/

void mostraTemperatura();
void mostraVelocidade();
void mostraNivel();
void mostraHora();
void medirVolume();

void acionamentos();

void motorPasso();
void capturaOVF();
void capturaDescida();
void capturaSubida();
void resetWDT();

void problema(uint8_t tipo);

//Mede tamanho de vetores
#define tamVet(vet) (sizeof(vet)/sizeof((vet)[0]))

//Tipo para vetor de funcoes
typedef void (*funcoes)();

//vetor com funcoes
funcoes mostra[] = {mostraTemperatura, mostraVelocidade, mostraNivel, mostraHora};

//ponteiro para funcoes
uint8_t mostraPTR = 0;

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
  uint32_t ms2, ms10, ms60, ms500, s1;
  uint32_t testes;//para testes com tempo auxiliares
} time_t;
time_t temporizacao;

//Para medir volume de agua no reservatorio
class Volume {
  public:
    float milimetros, centimetros, metros;
    float litros, centilitros, mililitros;
    uint32_t captura;
    uint8_t ovf;
    uint8_t  test;
};
Volume reservatorio;

byte testeMotor = 0;

/**************************************************************************************************************************
                                      Inicializacao dos modulos do core Marcelino
***************************************************************************************************************************/

Digital digital;          //Controle dos pinos digital
Timer0  timer;            //Temporizacoes com o timer 0
Timer1  captura;          //Captura para leitura de largura de pulso do sensor de nivel com hardware do timer 1
Serial  serial;           //Modulo Serial para comunicacao com computador
Delay   delay;            //Um pequeno delay para o dispositivo HC-SR04 (sensor ultrassonico)
WDT     wdt;              //Watch Dog Timer (WDT) para vigiar o MCU e evitar travamentos
Timer2  motor;            //Timer 2 para temporizar o acionamento do motor de passo

/**************************************************************************************************************************
                                              Instancias de Objetos
***************************************************************************************************************************/

IHM8574 display(displayADDRESS);              //Display 16x2 com ci PCF8574
Relogio relogio(pt_br);                       //Relogio RTC com dispositivo DS3231, semana e mes em pt_br
Temperatura temperatura(pinLM35, 30);         //Temperaturas com sensor de temperatura do DS3231 e LM35, media de 30 leituras analogicas
Controle controle(relayADDRESS, INVERSO);     //Controle dos atuadores com logica inversa (dreno de corrente)
Teclado teclado(pinTeclado);                  //Leitura do teclado analogico


//Classe para controle do motor de passo
Passo passo(motorPA, motorPB, motorPC, motorPD, ANODO);
/*
   Fiz umas implementacoes iniciais para utilizar com o aplicativo do motor de passo
   mas o resto vou deixar com voces, leiam os metodos que a classe Passo possui
   e usem para implementar a aplicacao.

   metodos da classe Passo

   Passo nome(a,b,c,d,modo);
   Inicia o modulo com os pinos ligados no motor de passo e o modo de operacao (ANODO comum / CATODO comum)

   parada();
   Para o motor de passo desligando a alimentacao das bobinas, importante para reduzir o consumo de corrente

   horario();
   Gira motor de passo no sentido horario

   antihorario();
   Gira motor de passo no sentido antihorario

   automatico(minimo, maximo);
   Gira o motor de passo automaticamente de um valor minimo ate um valor maximo (-35mil ate +35mil)

   passos();
   Retorna o numero de passos que o motor ja deu (positivos ou negativos ate 35mil passos)

   passos(valor);
   Configura o numero de passos do motor (valor = positivos ou negativos ate 35mil passos

*/

/**************************************************************************************************************************
                                                   Funcoes principais
***************************************************************************************************************************/


//Funcao de configuracao do MCU
void setup() {

  //display.create(posicao da memoria grafica, linha do simbolo, interador para salvar as oito linhas da matriz)
  //Salva caracter do simbolo de graus celcius na posicao 0 da memoria grafica do display
  for (int i = 0; i < 8; i++)
    display.create(0, get_pgm(graus, i), i);

  //Liga o background do display
  display.background(ON);

  //configura os pinos do reles
  controle.configura(velocidade1, velocidade2, velocidade3, bombaDagua, direcaoVertical, livre1, livre2, pinSinalizacao);

  //Inicia com todos os reles desligado
  controle.parada();

  digital.pullup(4, encoderA, encoderB, encoderButton, pinfimdeCurso);
  digital.mode(13,OUTPUT);

  temporizacao.testes = timer.millis();

  //Inicia com a ventilacao fechada
  while (digital.read(pinfimdeCurso)) {
    passo.antihorario();
    delay.ms(4);
    passo.passos(0);
    if(timer.millis() - temporizacao.testes >= 10000)
      problema(1);
  }

  motor.period(tempoUsMotorPasso);
  motor.attach(OVF, motorPasso);

  wdt.config(INT_RESET);  //configura o watch dog timer (WDT) para resetar o MCU se ele travar e executa uma interrupcao antes de resetar
  wdt.timeout(W_8S);      //configura o estouro do WDT para 8 segundos
  wdt.attach(resetWDT);   //executa a funcao "resetWDT()" caso o WDT reinicie o MCU
  wdt.enable();           //habilita o WDT

}//fim da funcao setup

//Funcao para execucao do codigo em ciclo infinito.
void loop() {

  //Tarefa realizada a cada 10 milisegundo
  if ( ( timer.millis() - temporizacao.ms10 ) >= 10) {    //Testa se passou 10ms

    temperatura.atualiza();                               //Atualiza as leituras de temperatura
    acionamentos();                                       //Chama funcao de acoes de controle
    mostra[mostraPTR]();                                  //Chama funcao alocada na posicao do ponteiro mostra

    serial.println(passo.passos());                       //Teste de quantos passos foram dados pelo motor de passo

    temporizacao.ms10 = timer.millis();                   //Salva o tempo atual para nova tarefa apos 10ms

  }//fim da tarefa de 10ms

  //Tarefa realizada a cada 60 milisegundo
  if ( ( timer.millis() - temporizacao.ms60 ) >= 60) {    //Testa se passou 60ms

    teclado.liberar();                                    //Libera o teclado para nova leitura, o tempo de 60ms garante o debounce das teclas
    temporizacao.ms60 = timer.millis();                   //Salva o tempo atual para nova tarefa apos 60ms

  }//fim da tarefa de 60ms

  //Tarefa realizada a cada 500 milisegundo
  if ( ( timer.millis() - temporizacao.ms500 ) >= 500) {  //Testa se passou 500ms

    medirVolume();                                        //Atualiza a leitura de volume do reservatorio
    relogio.sinalizar();                                  //Sinaliza ajuste do relogio com blink da configuracao selecionada

    if (digital.ifclear(encoderButton)) {                 //Se botao do encoder pressionado
      testeMotor ^= (1<<0);                               //Liga ou desliga o movimento automatico do motor de passo
      testeMotor &= ~(1 << 1);
      controle.reles(livre1,TOGGLE);                      //Liga led de sinalizacao
    }
    else if(testeMotor & (1<<1)) {
      controle.reles(livre1,LOW);
      testeMotor &= ~(1<<1);
    }

    temporizacao.ms500 = timer.millis();                  //Salva o tempo atual para nova tarefa apos 500ms

  }//fim da tarefa de 500ms

  //Tarefa realizada a cada 1 segundo
  if ( ( timer.millis() - temporizacao.s1 ) >= 1000) {    //Testa se passou 1 segundo

    controle.sinalizar();                                 //Sinaliza nivel de agua, se reservatorio estiver com nivel alto desliga sinalizacao, se nao, pisca a sinalizacao
    temporizacao.s1 = timer.millis();                     //Salva o tempo atual para nova tarefa apos 1s

  }//fim da tarefa de 1s

  wdt.clear();                                            //Limpa o watch dog timer (WDT) para evitar reset

}//fim da funcao loop

/**************************************************************************************************************************
                                                Funcoes auxiliares
***************************************************************************************************************************/

//Implementacoes separadas em arquivos para faciliar a leitura e solucao de problemas
#include "mostraTemperatura.h"
#include "mostraVelocidade.h"
#include "mostraNivel.h"
#include "mostraHora.h"
#include "acionamentos.h"
#include "medirVolume.h"
#include "interrupcoes.h"
#include "erros.h"

