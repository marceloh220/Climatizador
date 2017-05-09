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

void problema(uint8_t tipo) {
  
	display.set(0,0);
	display.print("Erro, problema: ");
	display.set(0,1);

	switch(tipo) {
		
		case 1:
			controle.parada();
			display.print("Paletas Horiz.  ");
			break;
			
		case 2:
			controle.parada();
			display.print("Sensor de Nivel  ");
			break;

    case 3:
      controle.parada();
      display.print("CPU resetando   ");
      break;
			
		default:
			display.print("Desconhecido    ");
			break;
	}
}


#endif
