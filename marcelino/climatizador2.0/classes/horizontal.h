#ifndef _HORIZONTAL_H_
#define _HORIZONTAL_H_

#include "../hardware.h"
#include "passo.h"
#include "encoder.h"

#ifndef
#define
#endif

static Passo passo(motorPA, motorPB, motorPC, motorPD, ANODO);
/*
   Metodos da classe Passo

   Passo nome(a,b,c,d,modo);
   Inicia o modulo com os pinos ligados no motor de passo e o modo de operacao (ANODO comum / CATODO comum)

   parada();
   Para o motor de passo desligando a alimentacao das bobinas, importante para reduzir o consumo de corrente

   horario();
   Gira motor de passo no sentido horario

   antihorario();
   Gira motor de passo no sentido antihorario

   automatico(minimo, maximo);
   Gira o motor de passo automaticamente de um valor minimo ate um valor maximo (-35mil ate +35mil)

   passos();
   Retorna o numero de passos que o motor ja deu (positivos ou negativos ate 35mil passos)

   passos(valor);
   Configura o numero de passos do motor (valor = positivos ou negativos ate 35mil passos

*/

class Ventilacao {

	private:
		uint8_t teste = 0;

	public:

		inline void abrir() {
			if
		}
		
};

#endif
