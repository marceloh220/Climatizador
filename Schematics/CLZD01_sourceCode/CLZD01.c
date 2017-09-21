/**************************************************************************************************************************

   Autor: Marcelo Henrique Moraes
   E-mail: marceloh220@hotmail.com
   Copyright (c) 2017 Marcelo Henrique Moraes

   Projeto Casa Sustentavel: Climatizador de AR
   https://github.com/marceloh220/Climatizador.git

   Projeto core Marcelino
   https://github.com/marceloh220/Marcelino.git

   MCU: Attiny45
   WinAVR ver. 20100110
   AVR studio 4.18
   F_CPU 8MHz (Oscilador interno)

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

#include <avr/io.h>			//biblioteca com definicoes do dispositivo
#include <avr/interrupt.h>	//biblioteca para tratamento de interrupcoes
#include <stdint.h>			//biblioteca para tipos de veriaveis

#define bv(bit)	(1<<bit)				//criacao de mascaras
#define data()	(PORTB|=(1<<PB1))		//seta o pino de dado
#define ndata()	(PORTB&=~(1<<PB1))		//reseta o pino de dado
#define high()	(PINB&(1<<PB2))			//verifica se pino de clock esta setado
#define low()	(!(PINB&(1<<PB2)))		//verifica se pino de clock esta resetado
#define nop()	asm volatile ("nop")	//sem operacao para aguardar sincronismo

#define maxRead		100	//numero maximo de leituras, depende de quanta memoria RAM existe disponivel
						//a pilha de ponteiros resiste na memoria ram, deixe espaco para ele

//prototipo de funcoes
void inicializacao();	//configuracoes iniciais do sistema
uint16_t leitura();		//leitura do sensor

//variaveis para leitura de media
uint16_t lidos[maxRead];	//espaco de leituras anteriores
uint16_t volatile media;	//valor da leitura media

//variavel para envio de dados
uint8_t interadorEnvio;		//interador para envio dos dados

//interrupcao de pevencao de erros no envio
ISR(TIMER0_OVF_vect) {		//se o mestre travar a comunicacao por um tempo muito longo
   
	interadorEnvio = 0;		//reinicia a transmissao no proximo clock
   
}//fim da interrupcao de prevencao de erros no envio

//interrupcao de envio
ISR(INT0_vect) {		//interrupcao INT0 AVR

	nop();				//espera 1 ciclo (125ns)
   
	if (high()) {		//teste de clock contra ruidos
	   
		TCNT0 = 0;		//quando uma transmissao esta em andamento normal envita o overflow do timer0
	   
		//variaveis para envio, mantidas em cache para acesso rapido em AVR
		static uint16_t volatile envio;	//cache para envio de dados
		static uint8_t paridade;		//para calculo de paridade
	   
		//teste de primeiro envio
		if (interadorEnvio == 0)	//se primeiro envio
			envio = media;			//carrega temperatura medida no cache de envio
		
		//teste de 10 bits enviados
		if ( interadorEnvio < 10) {	//se nao foi enviado os 10 bits
		   
			if (envio & (1 << 9)) {	//verifica se o bit mais significativo vale 1
				data();				//se sim, envia nivel logico alto no pino de dados
				paridade++;			//incrementa a paridade
			}
			
			else ndata();		//se nao, envia nivel logico baixo no pino de dados
			   
			envio = envio << 1;	//desloca o dado para enviar o proximo bit
			interadorEnvio++;	//incrementa o interador de envio
			
		}//fim do teste de 10 bits enviados
		
		//calculo de paridade
		else {							//se foi enviado os 10 bits
			if ((paridade % 2)) data();	//se paridade for par, o decimo segundo bit sera 0
			else ndata();				//se paridade for impar, o decimo segundo bit sera 1
			interadorEnvio = 0;			//reinicia o interador no final do envio dos 10 bits
			
		}//fim do calculo de paridade
		
	}//fim do teste de clock contra ruidos
	
}//fim da interrupcao de envio

//funcao principal
int main() {
	inicializacao();
	while (1) {
	   
		//variaveis de leitura
		static uint16_t volatile soma;	//acumulador das leituras analogicas
		static uint8_t  atual;			//interador para o espaco de leituras anteriores
	
	        //algoritimo de media movel
	        uint16_t aux = leitura();	//realiza a leitura do sensor analogico
		soma -= lidos[atual];			//retira do acumulador uma leitura antiga
		soma += aux;					//faz o somatorio da nova leitura
		lidos[atual] = aux;				//salva no espaco de leituras a leitura atual
		atual++;						//incrementa o interador de leitura
	   
		media = soma / maxRead;		//tira a media das leituras
	   
		if (atual == maxRead)		//se o interador chegou no final do espaco de leituras
			atual = 0;				//reinicia o interador
		
	}//fim do loop infinito
	
	return 0;
	
}//fim da funcao principal

//funcao de inicializacao
void inicializacao() {
   
	//limpa o cache de leituras anteriores
	int i;
	for (i = 0; i < maxRead; i++)
		lidos[i] = 0;

	//comparador interno
	ACSR = bv(ACD);	//desabilita comparador interno

	//conversor analogico/digital
	DIDR0 = bv(ADC3D);				//desabilita pino diginal em ADC3(PB3)
	ADMUX = bv(REFS2)|(3<<REFS0)|3;	//conversor analogico lendo pino ADC3(PB3), tensao de referencia no pino Aref(PB0)

	//interrupcao para transmissao
	MCUCR = bv(ISC01) | bv(ISC00);	//configura interrupcao externa INT para deteccao da borda de subida
	GIMSK = bv(INT0);				//habilita interrupcao externa INT

	//timer0 para evitar bloqueio da transmissao
	TCCR0B = 2;			//prescale 1/8, overflow em (1/8e6*256*8=256us)
	TIMSK = bv(TOIE0);	//interrupcao no overflow do timer 0

	//pinos de comunicacao
	ndata();		//coloca nivel logico baixo no pino de dados
	DDRB = bv(PB1);	//configura pino serial de saida de dados
	
	//interrupcoes globais
	sei();		//habilita interrupcoes globais

}//fim da funcao de inicializacao

//funcao de leitura adc
uint16_t leitura() {
   
	ADCSRA = bv(ADEN)|bv(ADSC)|3;	//habilita conversor analogico e inicia conversao
	while (ADCSRA & bv(ADSC));		//aguarda final da conversao
	return ADC;						//retorna valor lido no sensor
	
}//fim da funcao de leitura adc
