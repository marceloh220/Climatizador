#ifndef _FECHAMENTO_H
#define _FECHAMENTO_H

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

void fechamento() {

  motor.period(OFF);  //Desliga o timer de temporizacao do motor de passo para realizar o desligamento

  //enquanto a chave fim de curso nao for fechada
  while (digital.read(pinfimdeCurso))
  {
    passo.antihorario();          //gira o motor de passo no sentido antihorario para fechar a ventilacao
    delay.us(tempoUsMotorPasso);  //aguarda um tempo para o posicionamento do motor durante a movimentacao
    //se o tempo for muito curto o motor nao consegue se posicionar corretamente e perde passos
    //Se o tempo for muito longo a movimentacao do motor de passo fica mais lenta

    wdt.reset();  //reseta o wdt para evitar reinicializacoes

    if (passo.passos() < -1600)  //se esta demorando muito para fechar
      erro(erroHorizontal);      //avisa sobre o erro

  }//fim do fechamento da ventilacao

  passo.passos(0);     //seta o sistema com o numero de passos fechado
  passo.parada();      //para o motor de passo apos concluir o fechamento
  encoder.posicao(0);  //zera posicao do motor de passo no encoder

  motor.period(tempoUsMotorPasso);  //Volta a ligar o timer do motor de passo

}

#endif
