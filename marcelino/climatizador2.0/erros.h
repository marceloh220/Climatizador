#ifndef _ERROS_H_
#define _ERROS_H_

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

//se foi detectado algum erro apresenta o erro no display
void erro(uint8_t tipo) {

  controle.parada();  //para todos atuadores
  passo.parada();     //para o motor de passo

  //indica que existe um problema
  display.set(0, 0);
  display.print("Erro! Problema: ");
  display.set(0, 1);

  //alguns problemas previstos
  switch (tipo) {

    case 1:
      display.print("Paletas Horiz.  ");
      break;

    case 2:
      display.print("Sensor de Nivel  ");
      break;

    case 3:
      display.print("CPU resetando   ");
      break;

    case 4:
      display.print("Motor de Passo  ");
      break;

    //se aconteceu um erro imprevisto
    default:
      display.print("Desconhecido    ");
      break;
  }//fim da selecao de erros

  //mantem a mensagem por um tepo na tela
  delay.ms(5000);

}//fim da funcao de erros

#endif
