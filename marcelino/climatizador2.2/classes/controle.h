#ifndef _CONTROLE_H
#define _CONTROLE_H

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

#include "reles.h"
#include "../hardware.h"

#define VELOCIDADE		1
#define VERTICAL		2
#define HORIZONTAL		3
#define NBAIXO			4
#define NALTO			5

class Controle: private Reles {

private:
    uint8_t _velocidade;
    uint8_t _vertical;
    uint8_t _sinalizacao;
	uint8_t pinos[8];

	void vertical() {
		if(_vertical)
			Reles::liga(this->pinos[4]);
		else
			Reles::desliga(this->pinos[4]);
	}
	
public:

	//construtores
	Controle(uint8_t endereco, uint8_t modo)
	{
		
		Reles::configura(endereco, modo);
		this->configura(0,1,2,3,4,5,6,7);
		this->parada();
		
	}//construtor com pinos da placa de rele padroes, passar endereco e modo de operacao

	Controle(uint8_t endereco, uint8_t modo, uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h)
	{

		Reles::configura(endereco, modo);
		this->configura(a,b,c,d,e,f,g,h);
		this->parada();
		
	}//construtor para configuracao de endereco, modo de operacao e pinos na placa de reles

	//metodos
	
	void configura(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h)
	{
		
		this->pinos[0] = a;
		this->pinos[1] = b;
		this->pinos[2] = c;
		this->pinos[3] = d;
		this->pinos[4] = e;
		this->pinos[5] = f;
		this->pinos[6] = g;
		this->pinos[7] = h;
		
	}//fim do metodo configura, configura pinos na placa de reles

	void parada()
	{

			Reles::parada();
			
	}//fim do metodo parada, desliga todos os dispositivos

    void velocidade(uint8_t vel)
    {
		
		this->_velocidade = vel;
		Reles::desliga(this->pinos[0]);
		Reles::desliga(this->pinos[1]);
		Reles::desliga(this->pinos[2]);
		
		switch (vel) {
			case 1:
				Reles::liga(this->pinos[0]);
				break;
			case 2:
				Reles::liga(this->pinos[1]);
				break;
			case 3:
				Reles::liga(this->pinos[2]);
				break;
			default:
				break;
		}//fim do switch/case
		
	}//fim do metodo velocidade, configura a velocidade da ventilacao(0/1/2/3)

	inline uint8_t velocidade()
	{
		return _velocidade;
	}//metodo retorna velocidade de ventilacao atual

    void trocar(uint8_t modo)
    {
		
		if(modo == VELOCIDADE) {				//Se pedido para trocar a velocidade de ventilacao
			this->_velocidade++;				//Avanca velocidade
			if (this->_velocidade == 4) {		//Se estiver na ultima velocidade
				this->_velocidade = 0;			//Desliga ventilacao
				Reles::desliga(this->pinos[4]);	//Mantem a movimentacao das paletas verticais de direcionamento de ar desligada 
			}
			else 								//Se ventilacao ligada			
				this->vertical();				//Atualiza estado da movimentacao das paletas verticais
			this->velocidade(_velocidade);		//Configura a velocidade
		}//fim to teste de velocidade, troca a velocidade da ventilacao
		
		else if(modo == VERTICAL) {				//Se pedido para trocar o estado da movimentacao das paletas verticais
			if (this->_velocidade > 0) {		//Se ventilacao ligada
				_vertical=!_vertical;			//Altera o estado das paletas verticais de direcionamento de ar
				this->vertical();
			}
			else 								//Se ventilacao desligada			
				Reles::desliga(this->pinos[4]);	//Mantem a movimentacao das paletas verticais de direcionamento de ar desligada 

		}//fim da vertical, liga ou desliga a movimentacao das paletas verticais de direcionamento de ar
		
	}//fim do metodo trocar

	void sinalizar()
	{
		
		if(this->_sinalizacao)					//Se sinalizacao acionada
			Reles::troca(pinos[7]);				//Pisca sinalizacao
			
		else 									//Se nao		
			Reles::desliga(pinos[7]);			//Desliga sinalizacao
			
	}//fim do metodo sinalizar

	void reservatorio(uint8_t estado)
	{
		
		if(estado == NBAIXO) {					//Se passado reservatorio com nivel baixo
			if(this->_velocidade>0)				//Se ventilacao ligada
				this->_sinalizacao = 1;			//Aciona sinalizacao
			else 								//Se nao			
				this->_sinalizacao = 0;			//Desliga sinalizacao
			Reles::desliga(pinos[3]);			//Desliga a bomba de agua
			
		}//fim do teste de nivel baixo
		
		if(estado == NALTO) {					//Se passado reservatorio com nivel alto
			if(this->_velocidade>0)				//Se ventilacao ligada
				Reles::liga(pinos[3]);			//Liga a bomba de agua
			else 								//Se nao
				Reles::desliga(pinos[3]);		//Desliga a bomba de agua
			this->_sinalizacao = 0;				//Desliga sinalizacao
		}//fim do teste de nivel alto
		
	}//fim do metodo reservatorio

	void reles(uint8_t pin, uint8_t modo) {
		if(modo == LOW)
			Reles::desliga(pinos[pin]);
		else if(modo == HIGH)
			Reles::liga(pinos[pin]);
		else if(modo == TOGGLE)
			Reles::troca(pinos[pin]);
	}
	
};

#endif
