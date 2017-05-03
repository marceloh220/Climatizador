#ifndef _RELES_H
#define _RELES_H

#include <twi.h>
#include <digital.h>

#ifndef DIRETO
#define DIRETO	1
#endif

class Reles: private TWI {
	
	private:
	
		uint8_t _rele;
		uint8_t _endereco;
		uint8_t _modo;
	
	public:
	
	
		Reles(uint8_t endereco, uint8_t modo = DIRETO) {
			this->_endereco = endereco;
			this->_modo = modo;
		}
		
		void configura(uint8_t endereco, uint8_t modo) {
			this->_endereco = endereco;
			this->_modo = modo;
		}
		
		envia(uint8_t rele) {
			this->_rele = rele;
			TWI::start();
			TWI::send(_endereco);
			TWI::send(rele);
			TWI::stop();
		}
		
		void liga(char rele) {
			if(this->_modo == DIRETO)
				sbi(this->_rele, rele);
			else
				cbi(this->_rele, rele);
			this->envia(this->_rele);
		}

		void desliga(char rele) {
			if(this->_modo == DIRETO)
				cbi(this->_rele, rele);
			else
				sbi(this->_rele, rele);
			this->envia(this->_rele);
		}

		void troca(char rele) {
			tbi(this->_rele, rele);
			this->envia(this->_rele);
		}
		
		uint8_t rele() {
			return this->_rele;
		}
		
};


#endif
