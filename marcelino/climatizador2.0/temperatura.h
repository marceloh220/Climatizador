#ifndef _TEMPERATURA_H
#define _TEMPERATURA_H

#include "media.h"
#include <DS3231.h>

class Temperatura: private DS3231, private Media {

  public:

    Temperatura(uint8_t pino = A0) {
      Media::pin(pino);
    }
    
    inline void update() {
      Media::update();
    }

    inline float ambiente() {
      return DS3231::temp();
    }

    float refrigerada() {
      Media::update();
      // 1.1/1023 = 0.00107526 mv (foi utilizado a tensao de referencia interna de 1,1V)
      // LM35 e um sensor de temperatura LINEAR onde 0.201mV = 20.1°C
      // portanto => 0.00107526mV = 0.107526°C
      return Media::read() * 0.107526;
    }

};

#endif
