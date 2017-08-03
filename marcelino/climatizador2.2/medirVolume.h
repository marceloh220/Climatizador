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

    //configura a o hardware para capturar a borda de descida
    captura.attach(CAPT, FALLING, capturaVolume);

    captura.timer(0);     //limpa o temporizador de captura
    captura.prescale(1);  //configura o prescaler da captura em 1/1

  }//fim do teste de borda de subida

  //se nao, foi detectado uma borda de descida
  else {

    captura.prescale(OFF);  //desliga o temporizador de captura
    captura.detach(OVF);    //desanexa o overflow da interrupcao
    captura.detach(CAPT);   //desanexa a captura da interrupcao

    //salva o valor capturado
    reservatorio.pulso = captura.capt() + reservatorio.overflow * 65535;

  }//fim do teste de borda de descida

}//fim da interruptao de captura

//Funcao que inicia a medicao do volume de agua do reservatorio
void medirVolume() {

  // == prepara o hardware de captura ===
  captura.config(NORMAL);  //configura o temporizador de captura em modo normal (16bits do timer 1)

  //limpa os overflows do temporizador para a proxima captura
  reservatorio.overflow = 0;

  //anexa a funcao capturaOVF na interrupcao de overflow do timer 1
  captura.attach(OVF, capturaOVF);
  //anexa a funcao capturaVolume na interrupcao de captura do timer 1 com deteccao da borda de subida
  captura.attach(CAPT, RISING, capturaVolume);

  // === aciona sensor ultrassom ===
  digital.mode(pinTrigger, OUTPUT);
  digital.mode(pinEcho, INPUT);

  digital.write(pinTrigger, OFF);
  delay.us(2);
  digital.write(pinTrigger, ON);
  delay.us(10);
  digital.write(pinTrigger, OFF);

  //espera o final da captura
  do {

    if (reservatorio.overflow > 10)  //se captura demorando muito
      erro(erroSensorNivel);         //indica erro no sensor

  } while (captura.attach());

  // == calculo das distancias do sensor ultrassonico ===
  /*

    tic = prescale / F_CPU
    Distancia = Largura do Pulso * Velocidade do Som / 2

    Largura do Pulso = prescale * captura / F_CPU

    Distancia = prescale * captura * Velocidade do Som / ( 2 * F_CPU )
    Distancia = captura * 1 * 340.29[m/s] / ( 2 * 16e6[s] )
    Distancia = captura * 1.06340625e-05 [m]

  */

  reservatorio.milimetros = reservatorio.pulso * 0.0106340625;

  //m = mm * 10e-3
  reservatorio.metros = reservatorio.milimetros / 1000;

  //subtrai a altura do reservatorio da distancia medida pelo sensor
  float alturaAgua = alturaReservatorio - reservatorio.milimetros;

  //Evita valores negativos para no nivel de agua
  if (alturaAgua < 0.0)
    alturaAgua = 0;

  //Calcula o volume do reservatorio com base nas caracteristicas fisicas
  reservatorio.mmCubico = alturaAgua * comprimentoReservatorio * larguraReservatorio;
  reservatorio.mCubico = reservatorio.mmCubico / 1000.0;

  //Comvercao para litros
  reservatorio.mililitros = reservatorio.mCubico;
  reservatorio.litros = reservatorio.mCubico / 1000;
  
  serial.print("Volume do resevatório: ");
  serial.print(reservatorio.litros);
  serial.print(" L");

  if ( reservatorio.litros < nivelMIN )	{  //Se o nivel de agua esta abaixo do predeterminado
    
    controle.reservatorio(NBAIXO);  //Indica nivel baixo do reservatorio
    serial.println(" BAIXO");
    serial.println("Abasteça o reservatório com água");
    
  }

  else if ( reservatorio.litros > nivelMED ) {  //Se o nivel de agua esta acima do predeterminado
    
    controle.reservatorio(NALTO);  //Indica nivel alto do reservatorio
    serial.println(" BOM");
    
  }

}//fim da funcao medirVolume

#endif
