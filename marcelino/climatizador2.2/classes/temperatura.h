#ifndef _TEMPERATURA_H
#define _TEMPERATURA_H

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

#include "MoveAverage.h"
#include <DS3231.h>
#include <delay.h>

class Temperatura: private DS3231, private MoveAverage, private Delay {

public:
	
	Temperatura(uint8_t pino = A0, uint8_t leituras = 20) {
		MoveAverage::config(pino, leituras, INTERNAL);
	}
	
	void atualiza() {
		MoveAverage::update();
	}
	
	float ambiente() {
		float _temp = DS3231::temp();
		return _temp;
	}
	
	float refrigerada() {
		uint16_t _media = MoveAverage::average();
		// 1.1/1023 = 0.00107526 mv (foi utilizado a tensao de referencia interna de 1,1V)
		// LM35 e um sensor de temperatura LINEAR onde 0.201mV = 20.1°C
		// portanto => 0.00107526mV = 0.107526°C
		return _media * 0.107526;
	}

};

#endif
