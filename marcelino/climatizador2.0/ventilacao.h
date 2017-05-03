#ifndef _VENTILACAO_H
#define _VENTILACAO_H

#include "reles.h"
#include "hardware.h"

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
      if (this->_velocidade == 4)
        this->_velocidade = 0;
      this->velocidade(_velocidade);
    }

};

#endif
