//metodo para realizar a movimentacao do motor de passo
	void movimento(uint8_t ffrr) {

		//quando pede para abrir
		if(ffrr == ABRE) {
			
			if(this->p_passos < 3000) {	//se passos nao chegaram ao fim
				this->abre();			//abre a ventilacao
				this->p_passos++;		//incrementa o numero de passos
			}
			
		}//fim do teste ABRE

		//quando pede para fechar
		else if(ffrr == FECHA) {
			
			if(this->p_passos>0) {		//se passos nao estao no inicio
				this->fecha();			//fecha a ventilacao
				this->p_passos--;		//decrementa o numero de passos
			}
			
		}//fim do teste FECHA

		//quando pede o fechamento completo da ventilacao
		else if(ffrr == FECHAMENTO) {
			
			while( PIND & (1 << 3) ) {	//enquanto chave fim de curso aberta
				this->fecha();			//fecha a ventilacao
				_delay_us(1500);
			}
			p_passos = 0;				//reinicia o numero de passos
			this->para();				//para o motor de passo
			
		}//fim do teste fechamento
			
	}//fim do metodo movimento
	
	void automatico() {

		if(this->test & (1 << 1)) {
			
			if(this->p_passos < 600)
				this->movimento(ABRE);
			else if(p_passos > 2900)
				this->movimento(FECHA);
				
			else if(this->test & (1 << 0))
				this->movimento(ABRE);
			else
				this->movimento(FECHA);
	
			if(this->p_passos == 2900)
				this->test &= ~(1 << 0);
			else if(this->p_passos == 600)
				this->test |= (1 << 0);
			
		}

		else
			this->para();
		
	}//fim do metodo automatico
	
	inline uint16_t passos() { return p_passos; }

	void automatico(uint8_t teste) {
		if(teste == HIGH)
			test |= (1 << 1);
		else if(teste == LOW)
			test &= ~(1 << 1);
		else if(teste == TOGGLE)
			test ^= ~(1 << 1);
	}
