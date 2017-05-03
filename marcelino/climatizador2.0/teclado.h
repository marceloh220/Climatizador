#ifndef _TECLADO_H
#define _TECLADO_H

#include <analog.h>


class Teclado: private Analog {

  private:
    uint8_t pino, tranca;

  public:

    Teclado(uint8_t pin = A0) {
      pino = pin;
    }

    char leitura() {
      if (this->tranca)
        return NULL;
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
    }
    
    inline void liberar() {
      if (Analog::read(pino) > 980)
        this->tranca = 0;
    }

};

#endif
