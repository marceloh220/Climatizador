#ifndef _DESLIGAMENTO_H_
#define _DESLIGAMENTO_H_

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

void desligamento() {

  //configura led do pino 13 para sinalizar que esta em modo , porque o pino 13? Nao sei!
  DDRB |= (1 << PB5);   //pino PB5 como saida
  PORTB |= (1 << PB5);  //pino PB5 ligado

  teste.reg(0); //limpa todas as flags de teste

  //para varias coisas do sistema

  controle.parada();        //para todos os atuadores
  display.background(OFF);  //desliga luz de fundo do display
  display.display(OFF);     //desliga o display

  wdt.disable();      //desabilita o WDT
  sleep.disable(ALL); //desabilita os perifericos internos

  //realiza o fechamento das paletas de ventilacao
  fechamento();

  //coloca os pinos de controle do motor de passo em alta impedancia, garante o total desligamento
  passo.desligar();

  //anexa uma funcao de ligamento programado na interrupcao INT0 para despertar o sistema depois que desligado
  external.attach(INT0, FALLING, ligamentoProgramado);

  sleep.sleep(POWERDOWN); //coloca o MCU em modo de economia de energia

}

#endif
