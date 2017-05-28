#ifndef _RELOGIO_H
#define _RELOGIO_H

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

#include <DS3231.h>

class Relogio: private DS3231 {
	
private:

	uint8_t _ajuste;
	uint8_t _blink;
	char string[5];
	
public:

	Relogio(uint8_t lng) {
		DS3231::language(lng);
	}
	
	void ajuste() {
		this->_blink &= ~bv(this->_ajuste++);
		if (this->_ajuste > 7)
			this->_ajuste = 0;
	}
	
	inline void posicao(uint8_t pos) {
		this->_ajuste = pos;
	}
	
	inline uint8_t posicao() {
		return this->_ajuste;
	}
	
	void sinalizar() {
		if(this->_ajuste == 0)
			this->_blink = 0;
		else
			this->_blink ^= bv(this->_ajuste);
	}
	
	void decrementa() {					//decrementa as datas
		int8_t aux;
		switch (this->_ajuste) {		//Testa as posicoes de ajuste
			case 1:						//Se cursor posicionado para ajuste de hora
				aux = DS3231::hour();   //Le hora do relogio RTC
				aux--;                  //Decrementa hora
				if (aux < 0)            //Se hora passou de 0
					aux = 23;           //Retorna hora para 23
				DS3231::hour(aux);      //Salva hora ajustada
				break;
			case 2:						//Se cursor posicionado para ajuste de minuto
				aux = DS3231::minute(); //Le minuto do relogio RTC
				aux--;                  //Decrementa minuto
				if (aux < 0)            //Se minuto passou de 0
					aux = 59;           //Retorna minuto para 59
				DS3231::minute(aux);    //Salva minuto ajustado
				break;
			case 3:						//Se cursor posicionado para ajuste de hora
				aux = DS3231::second(); //Le segundo do relogio RTC
				aux--;                  //Decrementa segundo
				if (aux < 0)            //Se segundo passou de 0
					aux = 59;           //Retorna segundo para 59
				DS3231::second(aux);    //Salva segundo ajustado
				break;
			case 4:						//Se cursor posicionado para ajuste de semana
				aux = DS3231::week();   //Le semana do relogio RTC
				aux--;                  //Decrementa semana
				if (aux < 1)            //Se semana for menor que 1 (domingo)
					aux = 7;            //Retorna semana para 7 (sabado)
				DS3231::week(aux);      //Salva semana ajustado
				break;
			case 5:						//Se cursor posicionado para ajuste de dia
				aux = DS3231::day();    //Le dia do relogio RTC
				aux--;                  //Decrementa dia
				if (aux == 0)           //Se dia for menor que 1
					aux = 31;           //Retorna dia para 31
				DS3231::day(aux);       //Salva dia ajustado
				break;
			case 6:						//Se cursor posicionado para ajuste de mes
				aux = DS3231::month();  //Le mes do relogio RTC
				aux--;                  //Decrementa mes
				if (aux == 0)           //Se mes for 0
					aux = 12;           //Retorna mes para 12
				DS3231::month(aux);     //Salva mes ajustado
				break;
			case 7:						//Se cursor posicionado para ajuste de ano
				aux = DS3231::year();   //Le ano do relogio RTC
				aux--;                  //Decrementa ano
				if (aux < 0)            //Se ano passou de 0
					aux = 99;           //Retorna ano para 99
				DS3231::year(aux);      //Salva ano ajustado
				break;
			default:					//Se cursor em qualquer outra posicao
				break;					//Fazer nada
		}//fim do switch
	
	}//fim de decremento
	
	void incrementa() {					//incrementa as datas
		int8_t aux;
		
		switch(this->_ajuste) {			//Testa as posicoes de ajuste

			case 1:						//Se cursor posicionado para ajuste de hora
				aux = DS3231::hour();   //Le hora do relogio RTC
				aux++;                  //Incrementa hora
				if (aux == 24)          //Se hora chegou em 24
					aux = 0;            //Retorna hora para 0
				DS3231::hour(aux);      //Salva hora ajustada
				break;//fim caso 1
				
			case 2:						//Se cursor posicionado para ajuste de minuto
				aux = DS3231::minute(); //Le minuto do relogio RTC
				aux++;                  //Incrementa minuto
				if (aux == 60)          //Se minuto chegou em 60
					aux = 0;            //Retorna minuto para 0
				DS3231::minute(aux);    //Salva minuto ajustado
				break;//fim caso 2
				
			case 3:						//Se cursor posicionado para ajuste de hora
				aux = DS3231::second(); //Le segundo do relogio RTC
				aux++;                  //Incrementa segundo
				if (aux == 60)          //Se segundo chegou em 60
					aux = 0;            //Retorna segundo para 0
				DS3231::second(aux);    //Salva segundo ajustado
				break;//fim caso 3
				
			case 4:						//Se cursor posicionado para ajuste de semana
				aux = DS3231::week();   //Le semana do relogio RTC
				aux++;                  //Incrementa semana
				if (aux > 7)            //Se semana passou de 7 (sabado)
					aux = 1;            //Retorna semana para 1 (domingo)
				DS3231::week(aux);      //Salva semana ajustado
				break;//fim caso 4
				
			case 5:						//Se cursor posicionado para ajuste de dia
				aux = DS3231::day();    //Le dia do relogio RTC
				aux++;                  //Incrementa dia
				if (aux > 31)           //Se dia maior que 31
					aux = 1;            //Retorna dia para 1
				DS3231::day(aux);       //Salva dia ajustado
				break;//fim caso 5
				
			case 6:						//Se cursor posicionado para ajuste de mes
				aux = DS3231::month();  //Le mes do relogio RTC
				aux++;                  //Incrementa mes
				if (aux > 12)           //Se mes passou de 12(dezembro)
					aux = 1;            //Retorna mes para 1(janeiro)
				DS3231::month(aux);     //Salva mes ajustado
				break;//fim caso 6
				
			case 7:						//Se cursor posicionado para ajuste de ano
				aux = DS3231::year();   //Le ano do relogio RTC
				aux++;                  //Incrementa ano
				if (aux > 99)           //Se ano passou de 99
					aux = 0;            //Retorna ano para 0
				DS3231::year(aux);      //Salva ano ajustado
				break;//fim caso 7
				
				default:				//Se cursor em qualquer outra posicao
				;						//Fazer nada
				break;//fim caso defoult

		}//fim do switch
		
	}//fim de incremento
	
// === Retorna datas e horarios ===

	char* hora() {								//transforma hora em uma string
		
		if(! (this->_blink & bv(1) ) ) {		//Testa se deve fazer blink hora
			int aux = DS3231::hour();			//Le hora do dispositivo RTC DS3231
			this->string[0] = (aux / 10) + '0';	//Converte dezena hora em caracter e salva na primeira posicao da string
			this->string[1] = (aux % 10) + '0';	//Converte unidade hora em caracter e salva na segunda posicao da sting
			this->string[2] = '\0';				//Salva caracter null para sinalizar fim da string
			return string;						//Retorna hora na string convertida
		}//fim do teste de blink
		
		else {									//Se minuto em blink
			this->string[0] = ' ';
			this->string[1] = ' ';
			this->string[2] = '\0';
			return string;						//Retorna string com espacos vazios
		}
			
	}//fim do metodo hora
	
	char* minuto() {							//transforma minuto em uma string
		
		if(! (this->_blink & bv(2) ) ) {		//Testa se deve fazer blink minuto
			int aux = DS3231::minute();			//Le minuto do dispositivo RTC DS3231
			this->string[0] = (aux / 10) + '0';	//Converte dezena minuto em caracter e salva na primeira posicao da string
			this->string[1] = (aux % 10) + '0';	//Converte unidade minuto em caracter e salva na segunda posicao da sting
			this->string[2] = '\0';				//Salva caracter null para sinalizar fim da string
			return string;						//Retorna minuto na string convertida
		}//fim do teste de blink
		
		else {									//Se minuto em blink
			this->string[0] = ' ';
			this->string[1] = ' ';
			this->string[2] = '\0';
			return string;						//Retorna string com espacos vazios
		}
		
	}//fim do metodo minuto
	
	char* segundo() {							//transforma segundo em uma string
		
		if(! (this->_blink & bv(3) ) ) {		//Testa se deve fazer blink segundo
			int aux = DS3231::second();			//Le segundo do dispositivo RTC DS3231
			this->string[0] = (aux / 10) + '0';	//Converte dezena segundo em caracter e salva na primeira posicao da string
			this->string[1] = (aux % 10) + '0';	//Converte unidade segundo em caracter e salva na segunda posicao da sting
			this->string[2] = '\0';				//Salva caracter null para sinalizar fim da string
			return string;						//Retorna segundo na string convertida
		}//fim do teste de blink
		
		else {									//Se minuto em blink
			this->string[0] = ' ';
			this->string[1] = ' ';
			this->string[2] = '\0';
			return string;						//Retorna string com espacos vazios
		}
		
	}//fim do metodo segundo
	
	char* semana() {							//Retorna semana em uma string
		
		if(! (this->_blink & bv(4) ) )		//Testa se deve fazer blink semana
			return DS3231::weekSTR();			//Retorna semana do dispositivo RTC DS3231

		else {									//Se minuto em blink
			this->string[0] = ' ';
			this->string[1] = ' ';
			this->string[2] = ' ';
			this->string[3] = '\0';
			return string;						//Retorna string com espacos vazios
		}
		
	}//fim do metodo semana
	
	char* dia() {								//Transforma dia em uma string
		
		if(! (this->_blink & bv(5) ) ) {		//Testa se deve fazer blink dia
			int aux = DS3231::day();			//Le dia do dispositivo RTC DS3231
			this->string[0] = (aux / 10) + '0';	//Converte dezena dia em caracter e salva na primeira posicao da string
			this->string[1] = (aux % 10) + '0';	//Converte unidade dia em caracter e salva na segunda posicao da sting
			this->string[2] = '\0';				//Salva caracter null para sinalizar fim da string
			return string;						//Retorna dia na string convertida
		}//fim do teste de blink
		
		else {									//Se minuto em blink
			this->string[0] = ' ';
			this->string[1] = ' ';
			this->string[2] = '\0';
			return string;						//Retorna string com espacos vazios
		}
			
	}//fim do metodo dia
	
	char* mes() {								//Retorna mes em uma string
		
		if(! (this->_blink & bv(6) ) )		//Testa se deve fazer blink mes
			return DS3231::monthSTR();			//Retorna mes do dispositivo RTC DS3231

		else {									//Se minuto em blink
			this->string[0] = ' ';
			this->string[1] = ' ';
			this->string[2] = ' ';
			this->string[3] = '\0';
			return string;						//Retorna string com espacos vazios
		}
		
	}//fim do metodo mes
	
	char* ano() {								//Transforma ano em uma string

		if(! (this->_blink & bv(7) ) ) {		//Testa se deve fazer blink ano
			char aux = DS3231::year();			//Le ano do dispositivo RTC DS3231
			this->string[0] = '2';				//Salva caracter na primeira posicao da string
			this->string[1] = '0';				//Salva caracter na segunda posicao da string
			this->string[2] = (aux / 10) + '0';	//Converte dezena ano em caracter e salva na terceira posicao da string
			this->string[3] = (aux % 10) + '0';	//Converte unidade ano em caracter e salva na quarta posicao da string
			this->string[4] = '\0';				//Salva caracter null para sinalizar fim da string
			return string;						//Retorna ano na string convertida
		}//fim do teste de blink

		else {									//Se minuto em blink
			this->string[0] = ' ';
			this->string[1] = ' ';
			this->string[2] = ' ';
			this->string[3] = ' ';
			this->string[4] = '\0';
			return string;						//Retorna string com espacos vazios
		}

	}//fim do metodo ano

};


#endif
