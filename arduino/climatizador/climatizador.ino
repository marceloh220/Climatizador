/*

   Autor: Marcelo Henrique Moraes
   E-mail: marceloh220@hotmail.com

   Codigo fonte e bibliotecas disponiveis em:
   https://github.com/marceloh220/Climatizador/tree/master/arduino

   Esquematicos do projeto em formato PDF (Apenas leitura) e DSN (simulação Proteus 7.7)
   https://github.com/marceloh220/Climatizador/tree/master/Schematics

   Arduino IDE 1.8.2
   Arduino/Genoino Uno (ATmega328/p)
   Cristal externo 16MHz

   Bibliotecas sem compatibilidade total com Familia Arduino
   Apenas para Arduinos com MCU ATmega168/328/328p:
   TWI
   IHM8575
   DS3231

    This application is a free software, you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.
    This application is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY, without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    https://www.gnu.org/licenses/gpl-3.0.en.html

*/



/****************************************************
               Bibliotecas
****************************************************/

//Biblioteca para o hardware TWI
#include <twi.h>

//Biblioteca para controlar o display com CI PCF8574
#include <IHM8574.h>

//Biblioteca para uso do CI DS3231 ( RTC + Sensor de temperatura )
#include <DS3231.h>


/****************************************************
              Macros para AVR
****************************************************/

#define sbi(_sfr,_bit) (_sfr|=(1<<_bit))    //Set Bit In, para setar o bit (_bit) no registrador (_sfr)
#define cbi(_sfr,_bit) (_sfr&=~(1<<_bit))   //Clear Bit In, para limpar o bit (_bit) no registrador (_sfr)
#define tbi(_sfr,_bit) (_sfr^=(1<<_bit))    //TOGGLE Bit In, para alterar o esdado do bit (_bit) no registrador (_sfr)

/****************************************************

   Bitwise

   var|= a     =>   var = var | a
   var &= ~a   =>   var = var & ~a
   var ^= a    =>   var = var ^ a

   | or bit a bit (operacao logica OU realizada entre cada bit)

     0b0101 0000
   | 0b0001 1001
     0b0101 1001

   & and bit a bit (operacao logica E realizada entre cada bit)

     0b0001 1001
   & 0b1001 1000
     0b0001 1000

   ^ xor bit a bit (operacao logica OUEXCLUSIVA realizada entre cada bit)

     0b0101 1001
   ^ 0b1001 1000
     0b1100 0001

   ~ complemento (inverte o estado de cada bit)

     0b0101 0100
    ~0b1010 1011

   << shift left (move bit para a esquerda)

     0b0100 0101 << 3    (move os bits 3 casas para a esquerda)
     0b0010 1000

     (1<<3) => 0b0000 1000
     (3<<1) => 0b0000 0110


   >> shift right (move bit para a direita)

     0b0100 0101 >> 2    (move os bits 2 casas para a esquerda)
     0b0001 0001

     (128>>3) => 0b0001 0000
     (160>>0) => 0b1010 0000

*****************************************************/


/****************************************************
              Definicoes de Hardware
****************************************************/

//Endereco TWI do CI PCF8574A para controle dos reles
// 0100 A2 A1 A0 R/~W
#define relayADDRESS 0b01000010

//Endereco TWI do CI PCF8574A para display
// 0100 A2 A1 A0 R/~W
#define displayADDRESS 0b01000000

#define DESLIGADO LOW
#define LIGADO    HIGH

//Teclas
#define B0          0
#define B1          1
#define B2          2
#define B3          3
#define B4          4
#define B5          5
#define B6          6
#define B7          7
#define B8          8
#define NENHUMA     9     //Quando nenhuma tecla pessionada

#define pinTeclado A3     //Pino de leitura do teclado analogico
#define pinLED     13     //Pino de LED on board da placa Arduino


/****************************************************
              Instancias de Objetos
****************************************************/

//Display instanciado com o nome "display"
IHM8574 display(displayADDRESS);

//DS3231 instanciado com o nome "sensor"
DS3231 sensor;

//Controle do Hardware TWI instanciado com o nome "twi"
TWI twi;

/****************************************************
           Prototipo de funcoes auxiliares
****************************************************/

//Mostra no display a temperatura lida do DS3231
void mostraTemperatura();

//Mostra no display o valor analogico do teclado lido pelo pino A3
void mostraTeclado(char tecla);

//Funcao que envia o estado dos reles para a placa de controle
void enviarRele(char rele);

//Funcao de ligar  os reles
void ligarRele(char rele);

//Funcao de alterar estado os reles
void mudarRele(char rele);

//Funcao de desligar os reles
void desligarRele(char rele);

//Funcao de leitura das teclas
char leituraTeclado();

/****************************************************
               Constantes do sistema
****************************************************/

//Macro de leitura de caracter salvo na memoria de programa
#define get_pgm(m,p)   pgm_read_byte(&m[p])

//Caracter simbolo de graus para display 16x2 de matriz 5x8
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

//Caracter simbolo de rostinho feliz para display 16x2 de matriz 5x8
const uint8_t rostinho[8] PROGMEM =
{
  0b00010001,
  0b00000000,
  0b00000000,
  0b00010001,
  0b00010001,
  0b00001110,
  0b00000000,
  0b00000000
};

/****************************************************
               Variaveis de sistema
****************************************************/

//Variavel de controle dos reles
char relay = 0;

//Variavel de controle do tempo
unsigned long tempTecladoDisplay, tempLED;

/****************************************************
               Funcoes principais
****************************************************/

//Funcao Arduino de configuracao do MCU
void setup()
{

  //display.creat(posicao da memoria grafica, linha do simbolo, interador para salvar as oito linhas da matriz)

  //Salva caracter do simbolo de graus celcius na posicao 1 da memoria grafica do display
  for (int i = 0; i < 8; i++)
    display.create(0, get_pgm(graus, i), i);

  //Salva caracter rostinho feliz na posicao 2 da memoria grafica do display
  for (int i = 0; i < 8; i++)
    display.create(1, get_pgm(rostinho, i), i);

  //Liga o background do display
  display.background(LIGADO);

  //Pino LED como saida
  pinMode(pinLED, OUTPUT);

}//fim da funcao setup

//Funcao Arduino para execucao do codigo em ciclo infinito
void loop()
{

  //Tarefa realizada a cada 200 milisegundo
  //Testa se passou 200ms
  if ( millis() - tempTecladoDisplay >= 200) {

    byte lido = leituraTeclado();   //Realiza a leitura do teclado analogico

    display.set(0, 0);              //Posiciona cursor do display na coluna 0/linha 0
    mostraTemperatura();            //Chama funcao de mostrar temperatura no display
    display.set(0, 1);              //Posiciona cursor do display na coluna 0/linha 1
    mostraTeclado(lido);            //Chama funcao de mostrar leitura do teclado no display

    mudarRele(lido);                //Altera o estado do rele com botao apertado

    tempTecladoDisplay = millis();  //Salva o tempo atual para nova tarefa apos 200ms

  }//fim do test de 200ms

  //Tarefa realizada a cada 1 segundo
  //Testa se passou 1 segundo
  if ( millis() - tempLED >= 1000) {

    if (leituraTeclado() == B8)     //Se o botao B8 foi pressionado
      digitalWrite(pinLED, HIGH);   //Liga o led no pino 13

    else                            //Se nao
      tbi(PORTB, PB5);              //Pisca led do pino 13, acesso direto ao PORTB alterando somente o pino PB5

    tempLED = millis();             //Salva o tempo atual para nova tarefa apos 1s

  }//fim do test de 1s


}//fim da funcao loop


/****************************************************
               Funcoes auxiliares
****************************************************/

//Mostra no display a temperatura lida do DS3231
void mostraTemperatura()
{
  display.print("Temp: ");            //Mostra string no display
  float temperatura = sensor.temp();  //Realiza a leitura da temperatura do sensor
  display.print(temperatura);         //Mostra temperatura no display
  display.write((int)0);              //Mostra caracter salvo na posicao 0 da memoria grafica do display (simbolo graus)
  display.print("C  ");               //Mostra caracter no display
  display.write((int)1);              //Mostra caracter salvo na posicao 1 da memoria grafica do display (rostinho feliz)
}//fim da funcao mostraTemperatura

//Mostra no display o valor analogico do teclado lido pelo pino pinTeclado
void mostraTeclado(char tecla)
{
  display.print("Tecla: ");               //Mostra string no display
  int teclado = analogRead(pinTeclado);   //Realiza a leitura do teclado
  if (tecla == NENHUMA)                   //Se nenhuma tecla pessionada
    display.print("NENHUMA");             //Mostra string no display
  else                                    //Se nao
    display.print((int)tecla);            //Mostra valor lido no teclado
  display.print("      ");                //Mostra string no display
}//fim da funcao mostraTeclado

//Funcao que envia o estado dos reles para a placa de controle
void enviarRele(char rele) {
  twi.start();              //Condicao inicial de transmissao de dados por TWI
  twi.send(relayADDRESS);   //Envia o endereco do CI PCF8574 que controla os reles
  twi.send(rele);           //Envia o estado dos reles
  twi.stop();               //Condicao de parada de transmissao de dados por TWI
}//fim da funcao enviarRele

//Funcao de ligar  os reles
void ligarRele(char rele)
{
  sbi(relay, rele);         //Liga o rele passado
  enviarRele(relay);        //Envia o estado dos reles para a placa de controle

}//fim da funcao ligarRele

//Funcao de alterar estado os reles
void mudarRele(char rele)
{
  tbi(relay, rele);         //Alterar o estado do rele passado
  enviarRele(relay);        //Envia o estado dos reles para a placa de controle
}//fim da funcao desligarRele

//Funcao de desligar os reles
void desligarRele(char rele)
{
  cbi(relay, rele);         //Desliga o rele passado
  enviarRele(relay);        //Envia o estado dos reles para a placa de controle
}//fim da funcao desligarRele

//Funcao de leitura das teclas
char leituraTeclado()
{
  int teclado = analogRead(pinTeclado);   //Realiza a leitura do teclado analogico
  if (teclado < 100)                      //Testa teclas e retorna tecla pressionada
    return 0;
  else if (teclado < 200)
    return 1;
  else if (teclado < 330)
    return 2;
  else if (teclado < 410)
    return 3;
  else if (teclado < 460)
    return 4;
  else if (teclado < 510)
    return 5;
  else if (teclado < 560)
    return 6;
  else if (teclado < 600)
    return 7;
  else if (teclado < 700)
    return 8;

  else                                //Se nenhuma tecla pressionada
    return NENHUMA;
}
