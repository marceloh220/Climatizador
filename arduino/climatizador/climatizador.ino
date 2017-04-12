//Biblioteca para o hardwate TWI
TWI twi;
Delay delay;
Analog analog;

//Endereco TWI do CI PCF8574A para controle dos reles
// 0100 A2 A1 A0 R/~W
#define relayADDRESS 0b01000010


//Biblioteca Endereco TWI do CI PCF8574A para display 16x2
#include <IHM8574.h>
// 0100 A2 A1 A0 R/~W
#define displayADDRESS 0b01000000

IHM8574 display(displayADDRESS);


//Biblioteca para uso do CI DS3231
#include <DS3231.h>
DS3231 sensor;


//Macro de leitura de caracter salvo na memoria de programa
#define get_symbol(p)   pgm_read_byte(&symbol[p])
//Caracter simbolo de graus para display 16x2 de matriz 5x8
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

//Variavel de controle dos reles
char relay = 0;

//Funcao de ligar  os reles
void ligarrele(char rele) 
{
  relay |= (1 << rele);     //Operacao logica para ligar o rele passado
  twi.start();              //Condicao inicial de transmissao de dados por TWI
  twi.send(relayADDRESS);   //Envia o endereco do CI PCF8574 que controla os reles
  twi.send(relay);          //Envia o estado dos reles
  twi.stop();               //Condicao de parada de transmissao de dados por TWI
}

//Funcao de desligar os reles
void desligarrele(char rele) 
{
  relay &= ~(1<<rele);      //Operacao logica para ligar o rele passado
  twi.start();              //Condicao inicial de transmissao de dados por TWI
  twi.send(relayADDRESS);   //Envia o endereco do CI PCF8574 que controla os reles
  twi.send(relay);          //Envia o estado dos reles
  twi.stop();               //Condicao de parada de transmissao de dados por TWI
}


//Funcao Arduino de configuracao do MCU
void setup() 
{

  //Salva caracter do simbolo de graus celcius na memoria grafica do display
  for (int i = 0; i < 8; i++)
    display.create(1, get_symbol(i), i);

  display.background(ON);   //Liga o background do display 16x2

}

void loop() 
{

  display.set(0, 0);
  display.print("Temp: ");
  display.print(sensor.temp());
  display.write(1);
  display.print('C');

  display.set(0, 1);
  display.print("Analogico: ");
  display.print(analog.read(A3));
  display.print("   ");

  delay.ms(200);

}
