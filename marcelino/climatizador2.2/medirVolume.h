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
//regista os estouros do tmeporizador de captura
void capturaOVF() {
  reservatorio.overflow++;
}//fim da interrupcao de overflow

//realiza a captura
void capturaVolume() {

  //se detectado borda de subida
  if (captura.rising()) {
    
    //limpa o temporizador de captura
    captura.timer(0);
    //configura a o hardware para capturar a borda de descida
    captura.attach(CAPT, FALLING, capturaVolume);
    
  }//fim do teste de borda de subida

  //se nao, foi detectado uma borda de descida
  else {
    
    //salva o valor capturado
    reservatorio.pulso = captura.capt() + reservatorio.overflow * 65535;
    
    //limpa os overflows do temporizador para a proxima captura
    reservatorio.overflow = 0;
    
    captura.detach(OVF);  //desanexa o overflow da interrupcao
    captura.detach(CAPT);  //desanexa a captura da interrupcao
    
  }//fim do teste de borda de descida

}//fim da interruptao de captura

//Funcao que inicia a medicao do volume de agua do reservatorio
void medirVolume() {

  // == prepara o hardware de captura ===
  captura.config(NORMAL);  //configura o temporizador de captura em modo normal (16bits do timer 1)
  captura.prescale(1);     //configura o prescaler da captura em 1/1

  //anexa a funcao capturaVolume na interrupcao de captura do timer 1
  captura.attach(CAPT, RISING, capturaVolume);
  captura.attach(OVF, capturaOVF);

  // === aciona sensor ultrassom ===
  digital.write(pinUltrason, OFF);
  delay.us(2);
  digital.write(pinUltrason, ON);
  delay.us(10);
  digital.write(pinUltrason, OFF);

  //espera o final da captura
  do {

    if (reservatorio.overflow > 10)  //se captura demorando muito
      erro(erroSensorNivel);         //indica erro no sensor

  } while (captura.attach());

  // == calculo das distancias do sensor ultrassonico ===
  /*
    Distancia = Largura do Pulso * Velocidade do Som / 2

    Largura do Pulso = 1/F_CPU * prescale * captura

    Distancia = 1/F_CPU * prescale * captura * Velocidade do Som / 2
    Distancia = captura * 1/16e6[s] * 1 * 340.29[m/s] / 2
    Distancia = captura * 1.06340625e-05 [m]
  */
  
  reservatorio.milimetros = reservatorio.pulso * 0.0106340625;
  //serial.print("Largura de pulso: ");
  //serial.println(reservatorio.milimetros);

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
