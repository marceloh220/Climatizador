/*
 * Projeto automação casa alguma coisa
 *	Climatizador de Ar
 *
 * Marcelo H Moraes
 * marceloh220@hotmail.com
 *
 * Cores Marcelino disponivel em
 * htpps://github.com/marceloh220/Marcelino
 *
 * Codigo fontes do climatizador disponivel em
 * https://github.com/marceloh220/Climatizador
 *
 * Videos do desenvolvimento
 * https://www.youtube.com/watch?v=5_P18dyZHlA&list=PLAFROJL-VFf2txQNDm3TZ7mB_tUi9bb8s
 *
 *
 * Copyright (c) 2017 Marcelo Henrique Moraes
 *   This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 */

//Biblioteca com cores marcelino para MCU Atmega48/88/168/328
#include <marcelino.h>

#include <motordepasso.h>

Digital digital;
Delay delay;

Passo ventilacao;

Register test;

void setup() {

	digital.pullup(0);
	digital.pullup(1);
	digital.pullup(2);
	digital.pullup(3);

	ventilacao.fechado();
	
}

void loop() {

	if( digital.ifclear(0) ) {
		ventilacao.abre();
		test.clear(0);
	}
	else if( digital.ifclear(1) ) {
		ventilacao.fecha();
		test.clear(0);
	}
	else if( digital.ifclear(2) ) {
		test.toggle(0);
		delay.ms(250);
	}
	else if( digital.ifclear(3) ) {
		ventilacao.fechado();
		test.clear(0);
	}

	if( test.ifset(0) )
		ventilacao.automatico();

	delay.ms(2);

}
