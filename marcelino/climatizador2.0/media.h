#ifndef _MEDIA_H
#define _MEDIA_H

#include <analog.h>

struct Media : private Analog {
	
  private:
  
    uint16_t analog[100];
    uint16_t total;
    uint16_t media;
    uint16_t interator;
    uint8_t  _pin;
	uint8_t	 _reads;
    
  public:
  
    Media(uint8_t pino = A0, uint8_t reads = 20) {
      for (uint16_t i = 0; i < reads; i++)
        analog[i] = 0;
      this->_pin = pino;
      this->_reads = reads;
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
