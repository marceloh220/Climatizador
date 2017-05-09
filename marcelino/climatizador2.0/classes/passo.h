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

class Passo: private Digital {

private:

	int8_t passo;
	int16_t _passos;
	uint8_t pinos[4];
	uint8_t _modo;

	void configura(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t modo) {
		
		if(modo == CATODO)
			Digital::clear(4,a,b,c,d);
		else if(modo == ANODO)
			Digital::set(4,a,b,c,d);
			
		Digital::output(4,a,b,c,d);
		
		this->pinos[0] = a;
		this->pinos[1] = b;
		this->pinos[2] = c;
		this->pinos[3] = d;
		this->_modo = modo;
	}
	
public:

	//construtor
	Passo(uint8_t a = 0, uint8_t b = 1, uint8_t c = 2, uint8_t d = 3, uint8_t modo = CATODO)
	{

		this->configura(a,b,c,d,modo);
		
	}//fim do construtor

	//mantem o motor parado
	void parada()
	{
		
		if(this->_modo == CATODO) {
			for(int i = 0; i < 3; i++)
				Digital::write(this->pinos[i],LOW);
		}
		else if(this->_modo == ANODO) {
			for(int i = 0; i < 3; i++)
				Digital::write(this->pinos[i],HIGH);
		}
			
	}//fim do metodo parada

	//gira motor no sentido horario
	void horario()
	{
		
		if(this->_modo == CATODO)
			Digital::write(this->pinos[this->passo++],LOW);
		else if(this->_modo == ANODO)
			Digital::write(this->pinos[this->passo++],HIGH);

		if(this->passo == 4)
			this->passo = 0;
			
		if(this->_modo == CATODO)
			Digital::write(this->pinos[this->passo],HIGH);
		else if(this->_modo == ANODO)
			Digital::write(this->pinos[this->passo],LOW);

		this->_passos++;
		
	}//fim do metodo horario

	//gira motor no sentido antihorario
	void antihorario()
	{
		
		if(this->_modo == CATODO)
			Digital::write(this->pinos[this->passo--],LOW);
		else if(this->_modo == ANODO)
			Digital::write(this->pinos[this->passo--],HIGH);

		if(this->passo == -1)
			this->passo = 3;

		if(this->_modo == CATODO)
			Digital::write(this->pinos[this->passo],HIGH);
		else if(this->_modo == ANODO)
			Digital::write(this->pinos[this->passo],LOW);

		this->_passos--;
		
	}//fim do metodo antihorario

	//para verificar quantos passos foram dados
	inline int16_t passos()
	{
		
		return this->_passos;
		
	}//fim do metodo passos

	//para salvar numero de passos
	inline void passos(uint16_t value)
	{
		
		this->_passos = value;
		
	}//fim do metodo passos
		
};

#endif
