#ifndef _PASSO_H
#define _PASSO_H

/**************************************************************************************************************************

   Autor: Marcelo Henrique Moraes
   E-mail: marceloh220@hotmail.com
   Copyright (c) 2017 Marcelo Henrique Moraes

   Projeto Casa Sustentavel: Climatizador de AR
   https://github.com/marceloh220/Climatizador.git

   Projeto core Marcelino
   https://github.com/marceloh220/Marcelino.git

   MCU: Atmega328p
   Arduino IDE 1.8.2
   Marcelino Atmega328/p (Arduino/Genuino Uno/Nano)
   F_CPU 16MHz (Cristal externo)

    This application is a free software, you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.
    This application is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY, without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    https://www.gnu.org/licenses/gpl-3.0.en.html

***************************************************************************************************************************/

#include <defines.h>

//Uso do motor de passo com comum no negativo
#ifndef CATODO
#define	CATODO	1
#endif

//Uso do motor de passo com comum no positivo
#ifndef ANODO
#define ANODO	2
#endif

//Uso do motor de passo com movimentacao inversa
#ifndef INVERSO
#define INVERSO	1
#endif

//Uso do motor de passo com movimentacao direta
#ifndef DIRETO
#define DIRETO	2
#endif

class Passo {

private:

	int8_t _passo;
	int16_t _passosAndados;
	volatile uint8_t* port[4];
	volatile uint8_t* ddr[4];
	uint8_t bit[4];
	uint8_t _modo;
	uint8_t _sentido;

	void configura(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t modo) {

		this->bit[0]  = get_sfr(a);
		this->port[0] = get_PORT(this->bit[0]);
		this->ddr[0]  = get_DDR(this->bit[0]);
		this->bit[0]  = get_bit(a);
		
		this->bit[1]  = get_sfr(b);
		this->port[1] = get_PORT(this->bit[1]);
		this->ddr[1]  = get_DDR(this->bit[1]);
		this->bit[1]  = get_bit(b);
		
		this->bit[2]  = get_sfr(c);
		this->port[2] = get_PORT(this->bit[2]);
		this->ddr[2]  = get_DDR(this->bit[2]);
		this->bit[2]  = get_bit(c);

		this->bit[3]  = get_sfr(d);
		this->port[3] = get_PORT(this->bit[3]);
		this->ddr[3]  = get_DDR(this->bit[3]);
		this->bit[3]  = get_bit(d);
		
		this->_modo = modo;

		if(modo == CATODO) {
			for(int i = 0; i < 4; i++)
				*this->port[i] &= ~this->bit[i];
		}
		else if(modo == ANODO) {
			for(int i = 0; i < 4; i++)
				*this->port[i] |= this->bit[i];
		}
		
		for(int i = 0; i < 4; i++)
				*this->ddr[i] |= this->bit[i];
				
	}//fim do metodo configura
	
public:

	//construtor
	Passo(uint8_t a = 0, uint8_t b = 1, uint8_t c = 2, uint8_t d = 3, uint8_t modo = CATODO)
	{

		this->configura(a,b,c,d,modo);
		
	}//fim do construtor

	//mantem o motor parado
	inline void parada()
	{
		
		if(this->_modo == CATODO) {
			for(int i = 0; i < 4; i++)
				*this->port[i] &= ~this->bit[i];
		}
		else if(this->_modo == ANODO) {
			for(int i = 0; i < 4; i++)
				*this->port[i] |= this->bit[i];
		}
			
	}//fim do metodo parada

	//coloca saidas em alta impedancia
	void desligar() {
		for(int i = 0; i < 4; i++)
			*this->ddr[i] &= ~this->bit[i];
	}//fim do metodo desliga

	//tira as saidas da alta impedancia
	void ligar() {
		for(int i = 0; i < 4; i++)
			*this->ddr[i] |= this->bit[i];
	}//fim do metodo ligar

	//gira motor no sentido horario
	inline void horario()
	{
		
		if(this->_modo == CATODO)
			*this->port[this->_passo] &= ~this->bit[this->_passo];
		else if(this->_modo == ANODO)
			*this->port[this->_passo] |= this->bit[this->_passo];
			
		this->_passo++;
		if(this->_passo == 4)
			this->_passo = 0;
			
		if(this->_modo == CATODO)
			*this->port[this->_passo] |= this->bit[this->_passo];
		else if(this->_modo == ANODO)
			*this->port[this->_passo] &= ~this->bit[this->_passo];

		this->_passosAndados++;
		
	}//fim do metodo horario

	//gira motor no sentido antihorario
	inline void antihorario()
	{
		
		if(this->_modo == CATODO)
			*this->port[this->_passo] &= ~this->bit[this->_passo];
		else if(this->_modo == ANODO)
			*this->port[this->_passo] |= this->bit[this->_passo];

		this->_passo--;
		if(this->_passo == -1)
			this->_passo = 3;

		if(this->_modo == CATODO)
			*this->port[this->_passo] |= this->bit[this->_passo];
		else if(this->_modo == ANODO)
			*this->port[this->_passo] &= ~this->bit[this->_passo];

		this->_passosAndados--;
		
	}//fim do metodo antihorario

	//gira o motor automaticamente de um valor minimo ate um valor maximo
	inline void automatico(int min, int max, uint8_t modo = DIRETO) {
			
		if(this->_passosAndados < min) {
			if(modo == DIRETO)
				this->horario();
			else if(modo == INVERSO)
				this->antihorario();
		}
		else if(this->_passosAndados > max) {
			if(modo == DIRETO)
				this->antihorario();
			else if(modo == INVERSO)
				this->horario();
		}

		else if(this->_sentido) {
			if(modo == DIRETO)
				this->horario();
			else if(modo == INVERSO)
				this->antihorario();
		}
		else {
			if(modo == DIRETO)
				this->antihorario();
			else if(modo == INVERSO)
				this->horario();
		}

		if(this->_passosAndados == max)
			this->_sentido = 0;
		else if(this->_passosAndados == min)
			this->_sentido = 1;
			
	}//fim do metodo automatico

	//para verificar quantos passos foram dados
	inline int16_t passos()
	{
		
		return this->_passosAndados;
		
	}//fim do metodo passos

	//para salvar numero de passos
	inline void passos(uint16_t value)
	{
		
		this->_passosAndados = value;
		
	}//fim do metodo passos

	//metodo para posicionar o motor de passo
	inline void posicao(int16_t pos)
	{
			if(this->_passosAndados < pos)
				this->horario();
			else if(this->_passosAndados > pos)
				this->antihorario();
			else
				this->parada();
	}
		
};

#endif
