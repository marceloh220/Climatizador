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

***************************************************************************************************************************/

/**************************************************************************************************************************
                                                  Definicoes de Hardware
***************************************************************************************************************************/

// === Endereco TWI do CI PCF8574A (0100 A2 A1 A0 R/~W) ===

#define relayADDRESS (0b01000010)

#define displayADDRESS (0b01000000)


// === Nome para as constantes do reservatorio ===
// estao em milimetros e mililitros, alterar escalas em 'medirVolume.h'
#define nivelMIN                  100
#define nivelMED                  130
#define alturaReservatorio        60
#define comprimentoReservatorio   7.668711
#define larguraReservatorio       1


// === Pinos do Arduino ===

#define pinTeclado          A3        //pino de leitura do teclado analogico
#define pinLM35             A2        //pino de leitura do sensor de temperatura LM35

#define pinUltrason          9        //pino trigger do sensor de nivel ultrassonico
#define pinEcho              8        //pino de echo do sensor de nivel ultrassonico

// === Pinos do Relay ===

#define velocidade1         0         //rele de velocidade 1 do ventilador
#define velocidade2         1         //rele de velocidade 2 do ventilador
#define velocidade3         2         //rele de velocidade 3 do ventilador

#define bombaDagua          3         //rele de bomba de agua
#define direcaoVertical     4         //palhetas de direcionamento verticais da ventilacao

#define livre1              5         //rele livre
#define livre2              6         //rele livre

#define pinSinalizacao      7         //rele de led de sinalizacao

#endif

