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

  //aciona sensor ultrassom
  digital.write(pinUltrason, OFF);
  delay.us(2);
  digital.write(pinUltrason, ON);
  delay.us(10);
  digital.write(pinUltrason, OFF);

  uint32_t pulso = pulse.in(pinEcho,HIGH, 30);  //realiza a leitura da largura do pulso largura em ms

  /*
     Calculo das distancias do sensor ultrassonico
     
     Distancia[m] = Largura do Pulso[s] * Velocidade do Som[m/s] / 2
  */

  reservatorio.milimetros = pulso * 0.17145;
  //serial.print("Largura de pulso: ");
  //serial.println(reservatorio.milimetros);


  if (pulso == 0)
    reservatorio.testes++;
  else
    reservatorio.testes = 0;
 
  if (reservatorio.testes > 5)
    erro(erroSensorNivel);

  //cm = mm * 10e-2
  reservatorio.centimetros = reservatorio.milimetros * 1e-2;

  //m = mm * 10e-3
  reservatorio.metros = reservatorio.milimetros * 1e-3;

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
