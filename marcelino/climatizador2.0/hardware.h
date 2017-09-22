#ifndef _HARDWARE_H
#define _HARDWARE_H

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


    Perifericos utilizados:

    Entidade:
      D0(RX)/D1(TX): Comunicacao Serial RS232 com dispositivo USB/TTL FTDI FT232RL
      D2(INT0): Interrupcao INT0, coloca e tira a MCU (MicroController Unit) em modo de baixo consumo
      D3:D6: Controle motor de passo 35BYJ46, movimentacao das paletas horizontais
      D7: Micro switch de fim de curso do motor de passo
      D8/D9: Sensor ultrassonico HC-SR04 medidor de nivel de agua do reservatorio
      D10/D11: Leitura de encoder rotativo em modo de quadratura
      D12: Botao tactil do encoder de quadratura
      D13: Led de sinalizacao de MCU em modo de baixo consumo
      A2: Leitura analogica do sensor de temperatura LM35
      A3: Leitura analogica de teclado de 8 teclas montado com divisores de tensao
      A4/A5: Barramento de TWI (Two-Wire Interface), comunicacao com expansores de ports PCF8574 e dispositivo RTC (Real Time Clock) DS3231
      PC6: Reset do sistema
      PB6(X1)/PB7(X2): Cristal oscilador 16MHz
      A0: Entrada Analogica/ IO Digital livre
      A1: Entrada Analogica/ IO Digital livre

    Perifericos:
      BOD: 2.7V (Brown-Out Detection), reinicia se tensao de alimentacao ficar inferior a 2.7V
      WDT: 4 segundos (Watch Dog Timer), reinicia se CPU travar por mais de 4 segundos
      SLEEP: Modo de baixo consumo em POWERDOWN, com CPU e todos os perifericos desligados (exceto INT0)
      INT0: Interrupcao externa, coloca e tira a CPU do modo de baixo consumo
      Timer 0: Gera temporizacoes para o sistema, maior parte do sistema opera sem rotinas de atrazos
      Timer 1: Em uma versao futura ira realizar a captura da largura de pulso para medicao de nivel do reservatorio
      Timer 2: Gera constante de tempo para operacao do motor de passo e leitura do encoder rotativo
      ADC: Conversor Analogico Digital, realiza a leitura de A2 (sensor de temperatura) e A3 (teclado)
      TWI: Comunicacao a 2 fios para controle de atuadores e display LCD 16x2 com CI PCF8574, leitura do relogio de tempo real e temperatura com CI DS3231, Bit Rate 400kHz.
      UART: Comunicacao Assincrona com dois fios em protocolo RS232 (Universal Asynchronous Receiver/Transmitter), Baud Rate 9600, 8bits + 1bit parada sem bit de paridade.

    Reles PCF8574:
      Pino 0: Velocidade 1 da ventilacao
      Pino 1: Velocidade 2 da ventilacao
      Pino 2: Velocidade 3 da ventilacao
      Pino 3: Bomba de agua
      Pino 4: Direcionamento vertical
      Pino 5: Gerador de Anions
      Pino 6: Nada implementado (livre)
      Pino 7: Sinalização de reservatorio com nivel baixo
    

***************************************************************************************************************************/

// === Endereco TWI do CI PCF8574A (0100 A2 A1 A0 R/~W) ===

#define relayADDRESS (0b01000000)    //endereco do controle dos reles
#define displayADDRESS (0b01001110)  //endereco do display


// === Nome para as constantes do reservatorio ===
// escalas em milimetros e mililitros, alterar escalas em 'medirVolume.h'

//volume minimo e medio para o reservatorio
#define nivelMIN                  0.6
#define nivelMED                  1

//caracteristicas do reservatorio
#define alturaReservatorio        80
#define comprimentoReservatorio   255
#define larguraReservatorio       210


// === Pinos do Arduino ===

//pino INT0
#define pinPOWER             2  //pino do botao da interrupcao INT0 que coloca a cpu para dormir e acorda

//pinos analogicos
#define pinTeclado          A3  //pino de leitura do teclado analogico
#define pinLM35             A2  //pino de leitura do sensor de temperatura LM35

//pinos do sensor de nivel de agua do reservatorio
#define pinUltrason          9  //pino trigger do sensor de nivel ultrassonico
#define pinEcho              8  //pino de echo do sensor de nivel ultrassonico

//pinos de leitura do encoder
#define encoderA            10  //pino A do encoder para movimentacao horizontal da ventilacao
#define encoderB            11  //pino B do encoder para movimentacao horizontal da ventilacao
#define encoderButton       12  //pino do botao tactil do encoder

//pinos de controle do motor de passo
#define motorPA              3  //pino laranja
#define motorPB              4  //pino amarelo
#define motorPC              5  //pino branco (eu acho)
#define motorPD              6  //pino preto
#define pinfimdeCurso        7  //fim de curso

#define encoderEscala       20  //escala do encoder, muda o passo com que o encoder fecha ou abre as paletas de ventilacao horizontais
#define tempoUsMotorPasso 5000  //tempo da movimentacao do motor de passo em microsegundos


// === Pinos do Relay ===

//controle da velocidade da ventilacao principal
#define velocidade1         0  //rele de velocidade 1 do ventilador
#define velocidade2         1  //rele de velocidade 2 do ventilador
#define velocidade3         2  //rele de velocidade 3 do ventilador

#define bombaDagua          3  //rele de bomba de agua
#define direcaoVertical     4  //palhetas de direcionamento verticais da ventilacao
#define geradorAnion        5  //rele de gerador de anion

#define livre               6  //rele livre

//sinaliza nivel baixo do reservatorio
#define pinSinalizacao      7  //rele de led de sinalizacao

#endif

