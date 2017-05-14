// -----
// RotaryEncoder.h - Library for using rotary encoders.
// This class is implemented for use with the Arduino environment.
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
// -----
// 18.01.2014 created by Matthias Hertel
// -----

#ifndef RotaryEncoder_h
#define RotaryEncoder_h

#include "marcelino.h"

#define LATCHSTATE 3

const int8_t KNOBDIR[16] = {
  0, -1,  1,  0,
  1,  0,  0, -1,
  -1,  0,  0,  1,
0,  1, -1,  0  };

class RotaryEncoder: private Digital {

private:

	uint8_t _pin1, _pin2;	// Arduino pins used for the encoder. 
	int _oldState;
	int32_t _position;			// Internal position (4 times _positionExt)
	int32_t _positionExt;		// External position

public:

	RotaryEncoder(uint8_t pin1, uint8_t pin2) {
		// Remember Hardware Setup
		_pin1 = pin1;
		_pin2 = pin2;
		// Setup the input pins
		Digital::mode(pin1, INPUT);
		Digital::write(pin1, HIGH);   // turn on pullup resistor
		Digital::mode(pin2, INPUT);
		Digital::write(pin2, HIGH);   // turn on pullup resistor
		// when not started in motion, the current state of the encoder should be 3
		_oldState = 3;
		// start with position 0;
		_position = 0;
		_positionExt = 0;
	} // RotaryEncoder()
	
	
	inline long getPosition() {
		return _positionExt;
	} // getPosition()
	
	
	void setPosition(long newPosition) {
		// only adjust the external part of the position.
		_position = ((newPosition<<2) | (_position & 0x03L));
		_positionExt = newPosition;
	} // setPosition()
	
	
	inline void tick() {
		int sig1 = Digital::read(_pin1)? 1 : 0;
		int sig2 = Digital::read(_pin2)? 1 : 0;
		int8_t thisState = sig1 | (sig2 << 1);
		
		if (_oldState != thisState) {
			_position += KNOBDIR[thisState | (_oldState<<2)];
		
		if (thisState == LATCHSTATE)
			_positionExt = _position >> 2;
			_oldState = thisState;
		} // if
	} // tick()
  
};

#endif

// End
