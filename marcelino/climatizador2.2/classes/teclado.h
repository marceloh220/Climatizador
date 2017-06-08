#ifndef _TECLADO_H
#define _TECLADO_H

#include <defines.h>

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

class Teclado: private Analog {

  private:
    uint8_t pino, tranca;

  public:

    Teclado(uint8_t pin = A0) {
      pino = pin;
    }

    char leitura() {
      if (this->tranca)
        return 0;
      uint16_t _leitura = Analog::read(pino);

      if ( _leitura < 900)
        this->tranca = 1;
      if ( _leitura < 50)
        return 1;
      else if ( _leitura < 150)
        return 2;
      else if ( _leitura < 250)
        return 3;
      else if ( _leitura < 350)
        return 4;
      else if ( _leitura < 480)
        return 5;
      else if ( _leitura < 600)
        return 6;
      else if ( _leitura < 700)
        return 7;
      else if ( _leitura < 780)
        return 8;
      else if ( _leitura < 860)
        return 9;
      return 0;
    }
    
    inline void liberar() {
      if (Analog::read(pino) > 900)
        this->tranca = 0;
    }

};

#endif
