#ifndef _CLZD01_H_
#define _CLZD01_H_

#include <marcelino.h>

class CLZD: private Digital {

  private:
    uint8_t data, clock;
    uint16_t temp;

    void pulse() {
      Digital::write(clock, HIGH);
        _delay_us(8);
        Digital::write(clock, LOW);
        nop();
    }

    void com() {
      uint16_t volatile aux = 0;
      uint8_t paridade = 0;
      uint8_t j = 9;
      for (int i = 0; i < 10; i++) {
        this->pulse();
        if (Digital::read(data)) {
          _delay_us(1);
          if (Digital::read(data)) {
            aux |= (1<<j);
            paridade++;
          }
        }
        j--;
      }
      this->pulse();
      paridade %= 2;
      if (paridade == Digital::read(data)) {
        _delay_us(1);
        if (paridade == Digital::read(data))
          this->temp = aux;
      }
    }

  public:

    CLZD(uint8_t dataPin, uint8_t clockPin) {
      data = dataPin;
      clock = clockPin;
      Digital::write(clock, LOW);
      Digital::mode(clock, OUTPUT);
      Digital::mode(data, INPUT);
    }

    float read() {
      this->com();
      return this->temp * 0.2124;
    }

};


#endif
