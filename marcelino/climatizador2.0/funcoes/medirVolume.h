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

//Funcao que inicia a medicao do volume de agua do reservatorio
void medirVolume()
{
	
  digital.mode(pinUltrason, OUTPUT);                //pino de trigger do sensor de ultrasson configurado como saida
  captura.attach(OVF, medeVolumeOVF);               //anexa a funcao 'medeVolumeOVF' na interrupcao de overflow do temporizacao do hardware de captura
  captura.attach(CAPT, RISING, medeVolume);         //anexa a funcao 'medeVolume' na interrupcao de captura do MCU para detectar uma borda de subida do sinal no pino ICP
  digital.write(pinUltrason, ON);               	//liga o pulso de ultrassom do sensor HC-SR04
  delay.us(20);                                     //aguarda um tempo para que os pulsos ultrassonicos sejam enviados pelo sensor
  digital.write(pinUltrason, OFF);            		//desliga o pulso de ultrassom do sensor
  
  //calcula as distancias do sensor ultrassonico
  // Distancia = Largura do Pulso * Velocidade do Som / 2
  // Largura do Pulso = 1/F_CPU * prescale * captura
  // Distancia = 1/F_CPU * prescale * captura * Velocidade do Som / 2
  // Distancia = captura * 1/16e6[s] * 64 * 340.29[m/s] / 2
  // Distancia = captura * 0.00068058 [m]
  volume.metros = volume.captura * 0.00068058;
  
  //cm = m * 10e2
  volume.centimetros = volume.captura * 0.068058;
  
  //mm = m * 10e3
  volume.milimetros = volume.captura * 0.68058;
  
  //subtrai a altura do reservatorio da distancia medida pelo sensor
  float aux = alturaReservatorio - volume.milimetros;
  
  //Evita valores negativos para no nivel de agua
  if (aux < 0)
    aux = 0;
  
  //Calcula o volume do reservatorio com base nas caracteristicas fisicas dele
  volume.mililitros = aux * comprimentoReservatorio * larguraReservatorio;
  volume.centilitros = volume.mililitros / 10.0;
  volume.litros = volume.mililitros / 1000.0;
  
  //Se o nivel de agua esta abaixo do predeterminado
  if ( volume.mililitros < nivelMIN )
    rele.desliga(bombaDagua);                      //desliga a bomba d'agua
  
  //Se nao, se ventilacao ligada
  else if ( ventilacao.velocidade() ) {
    rele.desliga(pinSinalizacao);                  //desliga sinalizacao de nivel de agua baixo
    rele.liga(bombaDagua);                         //liga a bomba d'agua
  }
  
  //Se nao
  else {
    rele.desliga(pinSinalizacao);                  //desliga sinalizacao de nivel de agua baixo
    rele.desliga(bombaDagua);                      //desliga a bomba d'agua
  }

}//fim da funcao medirVolume

#endif
