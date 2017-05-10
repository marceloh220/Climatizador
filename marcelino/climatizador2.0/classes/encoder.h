#ifndef _ENCODER_H_
#define _EnCODER_H_

#include <defines.h>

class Encoder: private Digital {

private:

	uint8_t pinA;
	uint8_t pinB;
	uint8_t pinButton;

	int8_t atual;
	int8_t anterior;

	int8_t estados[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};

	void configura(uint8_t a, uint8_t b, uint8_t button) {
		this->pinA = a;
		this->pinB = b;
		this->pinButton = button;
		Digital::pullup(3,a,b,button);
	}//fim do metodo configura(a,b,button)

	void configura(uint8_t a, uint8_t b) {
		pinA = a;
		pinB = b;
		Digital::pullup(2,a,b);
	}//fim do metodo configura(a,b)

public:

	Encoder() {;;}
	Encoder(uint8_t a, uint8_t b, uint8_t button) { configura(a,b,button); }
	Encoder(uint8_t a, uint8_t b) { configura(a,b); }

	inline uint8_t encoder() {
		this->anterior <<= 2;
		this->anterior |= Digital::read(this->pinA);
		this->anterior <<= 1;
		this->anterior |= Digital::read(this->pinB);
		this->anterior &= 0x0F;
		return this->estados[this->anterior];
	}

	inline uint8_t botao() {
		return !Digital::read(pinButton);
	}

	
};


#endif
