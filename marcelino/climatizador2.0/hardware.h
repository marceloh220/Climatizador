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
// estao em milimetros e mililitros, alterar escalas em 'medirVolume.h'
#define nivelMIN                  100
#define alturaReservatorio        60
#define comprimentoReservatorio   7.668711
#define larguraReservatorio       1


// === Pinos do Arduino ===

#define pinTeclado          A3        //pino de leitura do teclado analogico
#define pinLM35             A2        //pino de leitura do sensor de temperatura LM35

#define pinUltrason          9        //pino trigger do sensor de nivel ultrassonico
#define pinEcho              8        //pino de echo do sensor de nivel ultrassonico

// === Pinos do Relay ===

#define velocidade1         0         //rele de velocidade 1 do ventilador
#define velocidade2         1         //rele de velocidade 2 do ventilador
#define velocidade3         2         //rele de velocidade 3 do ventilador

#define bombaDagua          3         //rele de bomba de agua
#define pinSinalizacao      7         //rele de led de sinalizacao

#endif
