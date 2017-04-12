/*
 * 
 * Autor: Marcelo Henrique Moraes
 * E-mail: marceloh220@hotmail.com
 * 
 * Codigo fonte e bibliotecas disponiveis em:
 * https://github.com/marceloh220/Climatizador/tree/master/arduino
 * 
 * Esquematicos do projeto em formato PDF (Apenas leitura) e DSN (simulação Proteus 7.7)
 * https://github.com/marceloh220/Climatizador/tree/master/Schematics
 * 
 * Arduino IDE 1.8.2
 * Arduino/Genoino Uno (ATmega328/p)
 * Cristal externo 16MHz
 * 
 * Bibliotecas sem compatibilidade total com Familia Arduino
 * Apenas para familia Arduino com MCU ATmega168/328/328p:
 * TWI
 * IHM8575
 * DS3231
 * 
 *  This application is a free software, you can redistribute it and/or 
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3.0 of the License, or (at your option) any later version.
 *  This application is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY, without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *  
 *  https://www.gnu.org/licenses/gpl-3.0.en.html
 * 
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
void mostraTeclado();

//Funcao de ligar  os reles
void ligarRele(char rele);

//Funcao de desligar os reles
void desligarRele(char rele);

/****************************************************
               Constantes do sistema
****************************************************/

//Macro de leitura de caracter salvo na memoria de programa
#define get_symbol(p)   pgm_read_byte(&symbol[p])

//Caracter simbolo de graus para display 16x2 de matriz 5x8
const uint8_t symbol[8] PROGMEM =
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


/****************************************************
               Variaveis de sistema
****************************************************/

//Variavel de controle dos reles
char relay = 0;


/****************************************************
               Funcoes principais
****************************************************/

//Funcao Arduino de configuracao do MCU
void setup()
{

  //Salva caracter do simbolo de graus celcius na posicao 1 da memoria grafica do display
  //display.creat(posicao da memoria grafica, linha do simbolo, interador para salvar as oito linhas da matriz)
  for (int i = 0; i < 8; i++)
    display.create(1, get_symbol(i), i);

  //Liga o background do display
  display.background(LIGADO);

}//end setup

void loop()
{

  //Interacao para efeito nos leds que representam os reles
  for (int i = 0; i < 8 ; i++) {
    display.set(0, 0);              //Posiciona cursor do display na coluna 0/linha 0
    mostraTemperatura();            //Chama funcao de mostrar temperatura no display
    display.set(0, 1);              //Posiciona cursor do display na coluna 0/linha 1
    mostraTeclado();                //Chama funcao de mostrar leitura do teclado no display
    ligarRele(i);                   //Liga o rele do interador
    delay(200);                     //Para por um tempo
    desligarRele(i);                //Desliga o rele do interador
  }

}//end loop


/****************************************************
               Funcoes auxiliares
****************************************************/

//Mostra no display a temperatura lida do DS3231
void mostraTemperatura()
{
  display.print("Temp: ");            //Mostra string no display
  float temperatura = sensor.temp();  //Realiza a leitura da temperatura do sensor
  display.print(temperatura);         //Mostra temperatura no display
  display.write(1);                   //Mostra caracter salvo na posicao 1 da memoria grafica do display (simbolo celcius)
  display.print('C');                 //Mostra caracter no display
}//end mostraTemperatura

//Mostra no display o valor analogico do teclado lido pelo pino A3
void mostraTeclado()
{
  display.print("Teclado: ");         //Mostra string no display
  int teclado = analogRead(A3);       //Realiza a leitura do teclado
  display.print(teclado);             //Mostra valor lido no teclado
  display.print("   ");               //Mostra string no display
}//end mostraTeclado

//Funcao de ligar  os reles
void ligarRele(char rele)
{
  relay |= (1 << rele);     //Operacao logica para ligar o rele passado
  twi.start();              //Condicao inicial de transmissao de dados por TWI
  twi.send(relayADDRESS);   //Envia o endereco do CI PCF8574 que controla os reles
  twi.send(relay);          //Envia o estado dos reles
  twi.stop();               //Condicao de parada de transmissao de dados por TWI
}//end ligarRele

//Funcao de desligar os reles
void desligarRele(char rele)
{
  relay &= ~(1 << rele);    //Operacao logica para ligar o rele passado
  twi.start();              //Condicao inicial de transmissao de dados por TWI
  twi.send(relayADDRESS);   //Envia o endereco do CI PCF8574 que controla os reles
  twi.send(relay);          //Envia o estado dos reles
  twi.stop();               //Condicao de parada de transmissao de dados por TWI
}//end desligarRele

