#ifndef _HARDWARE_H
#define _HARDWARE_H


/**************************************************************************************************************************
                                                  Definicoes de Hardware
***************************************************************************************************************************/

// === Endereco TWI do CI PCF8574A (0100 A2 A1 A0 R/~W) ===

#define relayADDRESS 0b01000010

#define displayADDRESS 0b01000000


// === Nomes diferenciados para as coisa ===

#define DESLIGADO LOW
#define LIGADO    HIGH
#define PISCANDO  TOGGLE

// === Nome para as constantes do reservatorio ===

#define nivelMIN                  100

#define alturaReservatorio        60
#define comprimentoReservatorio   7.668711
#define larguraReservatorio       1


// === Pinos do Arduino ===

#define pinTeclado          A3
#define pinLM35             A2

#define pinUltrason          9
#define pinEcho              8

// === Pinos do Relay ===

#define velocidade1         0
#define velocidade2         1
#define velocidade3         2

#define bombaDagua          3
#define pinSinalizacao      7

#endif
