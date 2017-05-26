#ifndef _MEDIR_VOLUME_H
#define _MEDIR_VOLUME_H

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

//interrupcao de deteccao de borda de subida do sinal no pino de captura
void capturaSubida() {
    captura.prescale(8);                        //Liga o temporizador de captura com prescale 8
    captura.attach(CAPT,FALLING,capturaDescida);//configura captura (CAPT) para detectar a borda de descida do sinal (FALLING)
}//fim da deteccao da borda de subida

//interrupcao de deteccao de borda de descida do sinal no pino de captura
void capturaDescida() {
    captura.prescale(OFF);                      //Desliga o temporizador da captura
    captura.detach(CAPT);                       //Para a interrupcao de captura (CAPT)
}//fim da deteccao da borda de descida

//Funcao que inicia a medicao do volume de agua do reservatorio
void medirVolume() {

	//prepara pino de trigger
	digital.mode(pinUltrason,OUTPUT);			//pino de trigger do sensor de ultrasson configurado como saida
	
	//prepara o temporizador de captura
	captura.timer(CLEAR);                       //Limpa o temporizador de captura
	
	//prepara captura
	captura.attach(CAPT,RISING,capturaSubida);
	//anexa a funcao 'capturaSubida' na interrupcao de captura (CAPT)
	//do MCU para detectar uma borda de subida (RISING) do sinal no pino ICP

	//aciona sensor ultrassom
	digital.write(pinUltrason,ON);				//liga o pulso de ultrassom do sensor HC-SR04
	delay.us(20);								//aguarda um tempo para que os pulsos de ultrassom sejam enviados pelo sensor
	digital.write(pinUltrason,OFF);				//desliga o pulso de ultrassom do sensor
	
	while(captura.attached(CAPT));				//Espera o fim da captura
	
	//calcula captura
	volume.captura = captura.capt();			//Salva o tempo de captura
	
	//Calcula as distancias do sensor ultrassonico
	// Distancia = Largura do Pulso * Velocidade do Som / 2
	// Largura do Pulso = 1/F_CPU * prescale * captura
	// Distancia = 1/F_CPU * prescale * captura * Velocidade do Som / 2
	// Distancia = captura * 1/16e6[s] * 8 * 340.29[m/s] / 2
	// Distancia = captura * 8.50725e-05 [m]
	volume.metros = volume.captura * 8.50725e-5;
	
	//cm = m * 10e2
	volume.centimetros = volume.captura * 8.50725e-3;
	
	//mm = m * 10e3
	volume.milimetros = volume.captura * 8.50725e-2;
	
	//subtrai a altura do reservatorio da distancia medida pelo sensor
	float aux = alturaReservatorio - volume.milimetros;
	
	//Evita valores negativos para no nivel de agua
	if (aux < 0)
		aux = 0;
	
	//Calcula o volume do reservatorio com base nas caracteristicas fisicas dele
	volume.mililitros = aux * comprimentoReservatorio * larguraReservatorio;
	volume.centilitros = volume.mililitros / 10.0;
	volume.litros = volume.mililitros / 1000.0;
	
	if ( volume.mililitros < nivelMIN )			//Se o nivel de agua esta abaixo do predeterminado
		controle.reservatorio(NBAIXO);			//indica nivel baixo do reservatorio
	
	else if( volume.mililitros > nivelMED )		//Se o nivel de agua esta acima do predeterminado
		controle.reservatorio(NALTO);			//indica nivel alto do reservatorio

}//fim da funcao medirVolume

#endif
