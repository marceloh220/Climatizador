#ifndef _ENCODER_H_
#define _EnCODER_H_

#include <defines.h>

class Encoder: private Digital {

private:

	//pinos
	uint8_t pinA;
	uint8_t pinB;
	uint8_t pinButton;

	//
	int16_t anterior;
	int32_t posTack;
	int32_t posEncoder;

	int8_t estados[16] = {
							0,-1,1,0,
							1,0,0,-1,
							-1,0,0,1,
							0,1,-1,0
						 };

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

	Encoder(uint8_t a, uint8_t b, uint8_t button) { configura(a,b,button); }
	Encoder(uint8_t a, uint8_t b) { configura(a,b); }

	//realiza a leitura da quadratura e retorna a quantidade de tacks
	int32_t encoder(uint16_t escala = 1) {
		
		uint8_t a = Digital::read(this->pinA);
		uint8_t b = Digital::read(this->pinB);

		int8_t atual = a | b<<1;

		if(this->anterior != atual) {
			
			this->posEncoder += this->estados[atual | (this->anterior << 2)];
		
			if (atual == 3) {
				this->posTack = this->posEncoder >> 2;
				this->posTack *= escala;
			}
		}
		
		return posTack;
		
	}//fim do metodo encoder

	//retorna a posicao
	inline uint32_t posicao() { return this->posTack; }

	//carrega novas posicoes
	void posicao(uint32_t pos) {
		this->posEncoder = ((pos << 2) | (this->posEncoder & 0x03L));
		this->posTack = pos;
	}

	//le o botao do encoder
	inline uint8_t botao() { return !Digital::read(pinButton); }

	
};//fim da classe encoder


#endif
