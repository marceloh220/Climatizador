#ifndef _VENTILACAO_H
#define _VENTILACAO_H

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

#include "reles.h"
#include "../hardware.h"

extern Reles rele(relayADDRESS, DIRETO);

class Ventilacao {

  private:
    uint8_t _velocidade;
    uint8_t _modo;

  public:

    void velocidade(uint8_t vel) {
		this->_velocidade = vel;
		rele.desliga(velocidade1);
		rele.desliga(velocidade2);
		rele.desliga(velocidade3);
		switch (vel) {
			case 1:
				rele.liga(velocidade1);
				break;
			case 2:
				rele.liga(velocidade2);
				break;
			case 3:
				rele.liga(velocidade3);
				break;
			default:
				break;
		}
	}

	inline uint8_t velocidade() {
		return _velocidade;
	}

    void trocar() {
      this->_velocidade++;
      if (this->_velocidade == 4) {
        this->_velocidade = 0;
        rele.desliga(direcaoVertical);
	  }
      this->velocidade(_velocidade);
    }

};

#endif
