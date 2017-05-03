#ifndef _MEDIA_H
#define _MEDIA_H

#include <analog.h>

#ifndef MediaMAX_READS
#define MediaMAX_READS 50
#endif

struct Media : private Analog {

  private:

    uint16_t analog[MediaMAX_READS];
    uint16_t total;
    uint16_t media;
    uint16_t interator;
    uint8_t  _pin;
    uint8_t	 _reads;

  public:

    Media(uint8_t pino = A0, uint8_t reads = 20) {
      if (reads > MediaMAX_READS)
        this->_reads = MediaMAX_READS;
      else
        this->_reads = reads;
      for (uint16_t i = 0; i < this->_reads; i++)
        analog[i] = 0;
      this->_pin = pino;
    }

    void update() {
      this->total -= this->analog[this->interator];
      this->analog[this->interator] = Analog::read(this->_pin, INTERNAL);
      this->total += this->analog[this->interator++];
      this->media = this->total / this->_reads;
      if (this->interator == this->_reads)
        this->interator = 0;
    }

    inline void pin(uint8_t pino) {
      this->_pin = pino;
    }

    inline void read(uint8_t reads) {
      this->_reads = reads;
    }

    inline uint16_t read() {
      return this->media;
    }

};

#endif
