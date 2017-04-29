 /*
  Copyright (c) 2017 Marcelo Henrique Moraes

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3.0 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

 */


#ifndef TIMER1_H
#define TIMER1_H

#include "Arduino.h"
#include <avr/io.h>

#ifndef OFF
#define OFF			0
#endif

#ifndef ON
#define ON			1
#endif

#ifndef CLEAR
#define CLEAR		0
#endif
#ifndef SET
#define SET			1
#endif
#ifndef CHANGE
#define CHANGE		2
#endif

#ifndef DISCONNECT
#define DISCONNECT	3
#endif
#ifndef	OVF
#define OVF			0
#endif
#ifndef COMPA
#define COMPA		1
#endif
#ifndef COMPB
#define COMPB		2
#endif
#ifndef CAPT
#define CAPT		3
#endif

#ifndef ICR
#define ICR			3
#endif
#ifndef OCRA
#define OCRA		1
#endif

#ifndef NORMAL
#define	NORMAL		0
#endif
#ifndef FAST
#define FAST		3
#endif
#ifndef CORRECT
#define CORRECT		1
#endif
#ifndef CTC
#define CTC			2
#endif

#ifndef FALLING
#define FALLING		6
#endif
#ifndef RISING
#define RISING		7
#endif

#ifndef CORRECT8
#define CORRECT8	1
#endif
#ifndef CORRECT9
#define CORRECT9	4
#endif
#ifndef CORRECT10
#define CORRECT10	5
#endif
#ifndef FAST8
#define FAST8		3
#endif
#ifndef FAST9
#define FAST9		6
#endif
#ifndef FAST10
#define FAST10		7
#endif
#ifndef VARIABLE
#define VARIABLE	8
#endif

#ifndef NINVERT
#define NINVERT		0
#endif
#ifndef INVERT
#define INVERT		1
#endif

#ifdef	T1
#undef	T1
#endif
#define T1			5

#ifdef	ICP
#undef	ICP
#endif
#define ICP			8

#ifdef	OC1A
#undef	OC1A
#endif
#define OC1A		9

#ifdef	OC1B
#undef	OC1B
#endif
#define OC1B		10

typedef void (*VoidFuncPtr)();

class Timer1 {
private:
	uint8_t def_top, def_mode2;
	uint16_t def_prescale;
	void variable(uint8_t mode, uint8_t top);
public:
	Timer1();
	
	//configs of timer
	void config(uint8_t mode, uint8_t mode2 = FAST, uint8_t top = CAPT);
	void prescale(uint16_t scale);
	void pinA(uint8_t mode);
	void pinB(uint8_t mode);
	
	//register of timer
	inline void timer(uint16_t value) { TCNT1 = value; }
	inline void compA(uint16_t value) { OCR1A = value; }
	inline void compB(uint16_t value) { OCR1B = value; }
	inline void capt(uint16_t value) { ICR1 = value; }
	inline uint16_t timer() { return TCNT1; }
	inline uint16_t compA() { return OCR1A; }
	inline uint16_t compB() { return OCR1B; }
	inline uint16_t capt() { return ICR1; }
	
	//pwm generator wave
	void pwmA(uint16_t value);
	void pwmA(uint16_t value, uint8_t mode);
	void pwmB(uint16_t value);
	void pwmB(uint16_t value, uint8_t mode);
	
	//control generator wave
	void frequency(uint32_t freq);
	void period(uint32_t micros);
	
	//interrupts of timer
	void attach(uint8_t interrupt, VoidFuncPtr funct);
	void attach(uint8_t interrupt, uint8_t mode, VoidFuncPtr funct);
	void detach(uint8_t interrupt);
	
	//test states
	inline uint8_t rising() { return (TCCR1B & (1<<ICES1)); }
	inline uint8_t falling() { return !(TCCR1B & (1<<ICES1)); }
	
};

#endif
