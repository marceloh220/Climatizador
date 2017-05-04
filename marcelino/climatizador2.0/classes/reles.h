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

#include <twi.h>
#include <digital.h>

#ifndef DIRETO
#define DIRETO	 1
#endif

#ifndef INVERSO
#define INVERSO  2
#endif

class Reles: private TWI {
	
	private:
	
		uint8_t _rele;
		uint8_t _endereco;
		uint8_t _modo;
	
	public:
	
	
		Reles(uint8_t endereco, uint8_t modo = DIRETO) {
			this->_endereco = endereco;
			this->_modo = modo;
		}
		
		void configura(uint8_t endereco, uint8_t modo) {
			this->_endereco = endereco;
			this->_modo = modo;
		}
		
		envia(uint8_t rele) {
			this->_rele = rele;
			TWI::start();
			TWI::send(_endereco);
			TWI::send(rele);
			TWI::stop();
		}
		
		void liga(char rele) {
			if(this->_modo == DIRETO)
				sbi(this->_rele, rele);
			else if(this->_modo == INVERSO)
				cbi(this->_rele, rele);
			this->envia(this->_rele);
		}

		void desliga(char rele) {
			if(this->_modo == DIRETO)
				cbi(this->_rele, rele);
			else if(this->_modo == INVERSO)
				sbi(this->_rele, rele);
			this->envia(this->_rele);
		}

		void troca(char rele) {
			tbi(this->_rele, rele);
			this->envia(this->_rele);
		}
		
		uint8_t rele() {
			return this->_rele;
		}
		
};


#endif
