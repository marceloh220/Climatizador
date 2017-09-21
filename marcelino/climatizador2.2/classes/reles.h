#ifndef _RELES_H
#define _RELES_H

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

//Classe TWI pertence ao core Marcelino
#include <twi.h>

//Uso dos reles com logica direta ( 0 = desligado, 1 = ligado )
#ifndef DIRETO
#define DIRETO	 1
#endif

//Uso dos reles com logica inversa ( 0 = ligado, 1 = desligado )
#ifndef INVERSO
#define INVERSO  2
#endif

class Reles: private TWI {

private:

	uint8_t _rele;			//controle dos reles
	uint8_t _endereco;		//endereco do dispositivo TWI
	uint8_t _modo;			//modo de operacao

public:

	Reles(uint8_t endereco = 0x00, uint8_t modo = DIRETO) {
		this->_endereco = endereco;
		this->_modo = modo;
	}//construtor com endereco e modo padroes
	
	void configura(uint8_t endereco, uint8_t modo) {
		this->_endereco = endereco;
		this->_modo = modo;
	}//metodo de configuracao de endereco e modo
	
	void envia(uint8_t rele) {
		TWI::start();
		TWI::send(_endereco);
		TWI::send(rele);
		TWI::stop();
	}//metodo de envio de dados para dispositivo TWI

	void parada() {
		if(this->_modo == DIRETO)
			this->_rele = 0;
		else if(this->_modo == INVERSO)
			this->_rele = 0xFF;
		this->envia(this->_rele);
	}//Metodo para desligar todos os reles
	
	void liga(char rele) {
		if(this->_modo == DIRETO)
			sbi(this->_rele, rele);
		else if(this->_modo == INVERSO)
			cbi(this->_rele, rele);
		this->envia(this->_rele);
	}//Metodo para ligar rele

	void desliga(char rele) {
		if(this->_modo == DIRETO)
			cbi(this->_rele, rele);
		else if(this->_modo == INVERSO)
			sbi(this->_rele, rele);
		this->envia(this->_rele);
	}//metodo para desligar rele

	void troca(char rele) {
		tbi(this->_rele, rele);
		this->envia(this->_rele);
	}//metodo para trocar o estad de rele
	
	uint8_t rele() {
		return this->_rele;
	}//metodo de retorno do estado dos reles
	
};


#endif
