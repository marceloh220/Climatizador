/*
  Copyright (c) 2017 Marcelo Henrique Moraes

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3.0 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

 */


#ifndef MOTORDEPASSO_H
#define MOTORDEPASSO_H

#include <defines.h>
#include <register.h>

const uint8_t PROGMEM passos_depasso[8] = 	{
									0b11111110,	//passo 0, inicio
									0b11111100,	//passo 1
									0b11111101,	//passo 2
									0b11111001,	//passo 3
									0b11111011,	//passo 4
									0b11110011,	//passo 5
									0b11110111,	//passo 6
									0b11110110,	//passo 7
								};
class Passo {

private:

	uint8_t passo = 0;
	uint16_t p_passos = 0;
	Register test;
	
	void close() {
		passo--;
		if(passo==255)
			passo=7;
		uint8_t aux = pgm_get(passos_depasso,passo);
		PORTB |= 0x0F;
		PORTB &= aux;
	}
	
	void open() {
		passo++;
		if(passo>7)
			passo = 0;
		uint8_t aux = pgm_get(passos_depasso,passo);
		PORTB |= 0x0F;
		PORTB &= aux;
	}
	
public:

	Passo() {
		PORTB |= 0x1F;
		DDRB |= 0x1F;
	}
	
	void abre() {
		if(p_passos<3000) {
			open();
			p_passos++;
		}
	}
	
	void fecha() {
		if(p_passos>0) {
			close();
			p_passos--;
		}
	}
	
	void automatico() {
		
		if(p_passos < 600)
			abre();
		else if(p_passos > 2900)
			fecha();
			
		else if( test.ifset(0) )
			abre();
		else
			fecha();

		if(p_passos == 2900)
			test.clear(0);
		else if(p_passos == 600)
			test.set(0);
	}
	
	void para() {
		PORTB |= 0x0F;
	}
	
	inline uint16_t passos() { return p_passos; }

	void fechado() {
		while( is_set(PINB,4) ) {
			close();
			_delay_us(1500);
		}
		p_passos = 0;
		para();
	}		
};
#endif
