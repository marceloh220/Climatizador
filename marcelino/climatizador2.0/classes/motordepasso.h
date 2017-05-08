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
#define ABRE 		1
#define FECHA		2
#define FECHAMENTO	4

const uint8_t PROGMEM passos_depasso[8] =
{
	0b11101111,	//passo 0, inicio
	0b11001111,	//passo 1
	0b11011111,	//passo 2
	0b10011111,	//passo 3
	0b10111011,	//passo 4
	0b00111111,	//passo 5
	0b01111111,	//passo 6
	0b01101111,	//passo 7
};

class Passo {

private:

	uint8_t passo = 0;
	uint16_t p_passos = 0;
	uint8_t test = 0;

	void fecha() {
		this->passo--;
		if(this->passo==255)
			this->passo=7;
		uint8_t aux = pgm_read_byte(&passos_depasso[this->passo]);
		PORTD |= 0xF0;
		PORTD &= aux;
	}
	
	void abre() {
		passo++;
		if(passo>7)
			passo = 0;
		uint8_t aux = pgm_read_byte(&passos_depasso[this->passo]);
		PORTD |= 0xF0;
		PORTD &= aux;
	}

	void para() {
		PORTD |= 0xF0;
	}
	
public:

	//construtor
	Passo() {

		//Pino PD4 a PD7 do PORTD como saidas para o driver do motor de passo
		PORTD |= 0xF0;		//Pinos PD4 a PD7 com nivel alto
		DDRD |= 0xF0;		//Pinos PD4 a PD7 como saidas

		//Pino PB2 do PORTB como entrada e resistor de pullUP interno para chave fim de curso
		PORTB |= bv(PB2);	//Pino PB2 com resistor de pullUP interno
		DDRB &= ~bv(PB2);	//Pino PB2 como entrada digital
		
	}//fim do construtor

	//metodo para realizar a movimentacao do motor de passo
	void movimento(uint8_t ffrr) {

		//quando pede para abrir
		if(ffrr == ABRE) {
			
			if(this->p_passos < 3000) {	//se passos nao chegaram ao fim
				this->abre();			//abre a ventilacao
				this->p_passos++;		//incrementa o numero de passos
			}
			
		}//fim do teste ABRE

		//quando pede para fechar
		else if(ffrr == FECHA) {
			
			if(this->p_passos>0) {		//se passos nao estao no inicio
				this->fecha();			//fecha a ventilacao
				this->p_passos--;		//decrementa o numero de passos
			}
			
		}//fim do teste FECHA

		//quando pede o fechamento completo da ventilacao
		else if(ffrr == FECHAMENTO) {
			
			while( PIND & (1 << 3) ) {	//enquanto chave fim de curso aberta
				this->fecha();			//fecha a ventilacao
				_delay_us(1500);
			}
			p_passos = 0;				//reinicia o numero de passos
			this->para();				//para o motor de passo
			
		}//fim do teste fechamento
			
	}//fim do metodo movimento
	
	void automatico() {

		if(this->test & (1 << 1)) {
			
			if(this->p_passos < 600)
				this->movimento(ABRE);
			else if(p_passos > 2900)
				this->movimento(FECHA);
				
			else if(this->test & (1 << 0))
				this->movimento(ABRE);
			else
				this->movimento(FECHA);
	
			if(this->p_passos == 2900)
				this->test &= ~(1 << 0);
			else if(this->p_passos == 600)
				this->test |= (1 << 0);
			
		}

		else
			this->para();
		
	}//fim do metodo automatico
	
	inline uint16_t passos() { return p_passos; }

	void automatico(uint8_t teste) {
		if(teste == HIGH)
			test |= (1 << 1);
		else if(teste == LOW)
			test &= ~(1 << 1);
		else if(teste == TOGGLE)
			test ^= ~(1 << 1);
	}
	
};
#endif
