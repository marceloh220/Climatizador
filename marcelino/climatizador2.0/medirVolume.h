#ifndef _MEDIR_VOLUME_H
#define _MEDIR_VOLUME_H

//Funcao que inicia a medicao do volume de agua do reservatorio
void medirVolume() {
  digital.mode(pinUltrason, OUTPUT);                //pino de trigger do sensor de ultrasson configurado como saida
  captura.attach(OVF, medeVolumeOVF);               //anexa a funcao 'medeVolumeOVF' na interrupcao de overflow do temporizacao do hardware de captura
  captura.attach(CAPT, RISING, medeVolume);         //anexa a funcao 'medeVolume' na interrupcao de captura do MCU para detectar uma borda de subida do sinal no pino ICP
  digital.write(pinUltrason, LIGADO);               //liga o pulso de ultrassom do sensor HC-SR04
  delay.us(20);                                     //aguarda um tempo para que os pulsos ultrassonicos sejam enviados pelo sensor
  digital.write(pinUltrason, DESLIGADO);            //desliga o pulso de ultrassom do sensor
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
  //Evita valores negativos para no nivel de agua (Claro! Seria negativo porque, o reservatorio ta devendo agua pra alguem?)
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
  else if ( ventilacao.velocidade()) {
    rele.desliga(pinSinalizacao);                  //desliga sinalizacao de nivel de agua baixo
    rele.liga(bombaDagua);                         //liga a bomba d'agua
  }
  else {
    rele.desliga(pinSinalizacao);                  //desliga sinalizacao de nivel de agua baixo
    rele.desliga(bombaDagua);                      //desliga a bomba d'agua
  }
}//fim da funcao medirVolume

#endif
