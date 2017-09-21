#ifndef _MOVEAVERAGE_H
#define _MOVEAVERAGE_H

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

//Class Analog in core Marcelino
#include <analog.h>
#include <serial.h>

#ifndef MoveAverageMAX_READS
#define MoveAverageMAX_READS 50
#endif

struct MoveAverage : private Analog, private Serial {

private:
	uint8_t reference;
	uint16_t _analog[MoveAverageMAX_READS];
	uint16_t _total;
	uint16_t _interator;
	uint16_t _average;
	uint8_t  _pin;
	uint8_t	 _reads;
	
public:
	
	MoveAverage(uint8_t pin = A0, uint8_t reads = 20, uint8_t ref = DEFAULT) {
		if (reads > MoveAverageMAX_READS)
			this->_reads = MoveAverageMAX_READS;
		else
			this->_reads = reads;
		for (uint16_t i = 0; i < this->_reads; i++)
			this->_analog[i] = 0;
		this->_pin = pin;
		this->reference = ref;
	}
	
	void config(uint8_t pin, uint8_t reads, uint8_t ref) {
		this->_pin = pin;
		this->_reads = reads;
		this->reference = ref;
	}
	
	void update() {
		this->_total -= this->_analog[this->_interator];
		this->_analog[this->_interator] = Analog::read(this->_pin, this->reference);
		this->_total += this->_analog[this->_interator++];
		this->_average = this->_total / this->_reads;
		if (this->_interator == this->_reads)
			this->_interator = 0;
	}
	
	uint16_t average() {
		return this->_average;
	}

};

#endif
