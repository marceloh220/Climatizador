#include <twi.h>
#include <IHM8574.h>
#include <DS3231.h>
#include <avr/pgmspace.h>

DS3231 clock;
IHM8574 display(64);
TWI twi;

#define get_symbol(p)   pgm_read_byte(&symbol[p])

const uint8_t symbol[8] PROGMEM = 
{
  0b00000110,
  0b00001001,
  0b00001001,
  0b00000110,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000
};

char relay = 0;

void ligarrele(char rele) 
{
  relay |= (1 << rele);
  twi.start();
  twi.send(65);
  twi.send(relay);
  twi.stop();
}

void desligarrele(char rele) 
{
  relay &= ~rele;
  twi.start();
  twi.send(65);
  twi.send(relay);
  twi.stop();
}

void setup() 
{

  for (int i = 0; i < 8; i++)
    display.create(1, get_symbol(i), i);

  display.background(1);

}

void loop() 
{

  display.set(0, 0);
  display.print("Temp: ");
  display.print(clock.temp());
  display.write(1);
  display.print('C');

  display.set(0, 1);
  display.print("Analogico: ");
  display.print(analogRead(A3));
  display.print("   ");

  delay(200);

}
