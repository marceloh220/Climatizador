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
void medirVolume() {

  reservatorio.test = 0;                              //Marca test como inicio de captura

  //prepara pino de trigger
  digital.mode(pinUltrason, OUTPUT);			            //Pino de trigger do sensor de ultrasson configurado como saida

  //prepara o temporizador de captura
  captura.timer(CLEAR);                               //Limpa o temporizador de captura
  reservatorio.ovf = 0;                               //Limpa registro de estouros do temporizador de captura

  //prepara captura
  captura.attach(CAPT, RISING, capturaSubida);
  //anexa a funcao 'capturaSubida' na interrupcao de captura (CAPT)
  //do MCU para detectar uma borda de subida (RISING) do sinal no pino ICP

  //aciona sensor ultrassom
  digital.write(pinUltrason, ON);				              //Liga o pulso de ultrassom do sensor HC-SR04
  delay.us(20);								                        //Aguarda um tempo para que os pulsos de ultrassom sejam enviados pelo sensor
  digital.write(pinUltrason, OFF);				            //Desliga o pulso de ultrassom do sensor

  delay.ms(40);                                       //Espera o tempo maximo da captura

  captura.detach(CAPT);                               //Para a interrupcao de captura (CAPT)
  captura.detach(OVF);                                //Para a interrupcao de overflow (OVF) do temporizador de captura

  //se captura realizada
  if(reservatorio.test) {

  /*
     Calculo das distancias do sensor ultrassonico

     Distancia = Largura do Pulso * Velocidade do Som / 2

     Largura do Pulso = 1/F_CPU * prescale * captura

     Distancia = 1/F_CPU * prescale * captura * Velocidade do Som / 2
     Distancia = captura * 1/16e6[s] * 1 * 340.29[m/s] / 2
     Distancia = captura * 1.06340625e-5 [m]

  */
  reservatorio.metros = reservatorio.captura * 1.06340625e-5;
  
  }
  
  //se nao, coloca sensor como nao lido
  else {
    problema(2);
    reservatorio.metros = alturaReservatorio;
    delay.ms(2000);
  }

  //cm = m * 10e2
  reservatorio.centimetros = reservatorio.metros * 1e2;

  //mm = m * 10e3
  reservatorio.milimetros = reservatorio.metros * 1e3;

  //subtrai a altura do reservatorio da distancia medida pelo sensor
  float alturaAgua = alturaReservatorio - reservatorio.milimetros;

  //Evita valores negativos para no nivel de agua
  if (alturaAgua < 0.0)
    alturaAgua = 0;

  //Calcula o volume do reservatorio com base nas caracteristicas fisicas
  reservatorio.mililitros = alturaAgua * comprimentoReservatorio * larguraReservatorio;
  reservatorio.centilitros = reservatorio.mililitros / 10.0;
  reservatorio.litros = reservatorio.mililitros / 1000.0;

  if ( reservatorio.mililitros < nivelMIN )		    //Se o nivel de agua esta abaixo do predeterminado
    controle.reservatorio(NBAIXO);			          //Indica nivel baixo do reservatorio

  else if ( reservatorio.mililitros > nivelMED )	//Se o nivel de agua esta acima do predeterminado
    controle.reservatorio(NALTO);			            //Indica nivel alto do reservatorio

}//fim da funcao medirVolume

#endif
