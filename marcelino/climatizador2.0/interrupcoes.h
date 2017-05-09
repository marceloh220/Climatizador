#ifndef _INTERRUPCOES_H_
#define _INTERRUPCOES_H_

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

// === Controle do motor de passo ===

void motorPasso()
{

  if (digital.ifclear(encoderA)) {            //quando encoder girando em um sentido

    if (digital.ifset(pinfimdeCurso))         //enquanto nao chegou no final
      passo.antihorario();                    //gira motor no sentido antihorario para fechar a ventilacao

    else {                                    //se chegou no final meio aberto
      passo.passos(0);
      passo.parada();                         //para o motor e poupa energia
    }

    testeMotor &= ~(1 << 0);
    testeMotor |= (1 << 1);

  }//fim do teste de encoder girando em um sentido

  else if (digital.ifclear(encoderB)) {       //quando encoder girando em outro sentido

    if (passo.passos() < 1500)                //enquanto nao chegou no limite de abertura
      passo.horario();                        //gira o motor no sentido horario para abrir a ventilacao

    else                                      //se chegou no limite de abertura
      passo.parada();                         //para o motor e poupa energia

    testeMotor &= ~(1 << 0);
    testeMotor |= (1 << 1);

  }//fim do teste de encoder girando em outro sentido

  else if (testeMotor & (1 << 0))             //se o teste de motor indicar movimentacao automatica
    passo.automatico(300, 1400);              //gira o motor de passo automaticamente de ventilacao quase fechada ate ventilacao aberta

  else                                        //quando nenhuma atividade no encoder
    passo.parada();                           //para o motor e poupa energia

}

// === Captura do sensor de nivel do reservatorio ===

//interrupcao de overflow do temporizador da captura
void capturaOVF() {

  reservatorio.ovf++;                             //Registra estouro do temporizador de captura

}//fim do overflow do temporizador de captura

//interrupcao de deteccao da borda de subida do sinal no pino de captura
void capturaSubida() {

  captura.prescale(1);                            //Liga o temporizador de captura com prescale 1
  captura.attach(CAPT, FALLING, capturaDescida);  //Configura captura (CAPT) para detectar a borda de descida do sinal (FALLING)
  captura.attach(OVF, capturaOVF);                //Ativa overflow (OVF) para detectar o estouro do temporizador de captura

}//fim da deteccao da borda de subida

//interrupcao de deteccao da borda de descida do sinal no pino de captura
void capturaDescida() {

  captura.prescale(OFF);                              //Desliga o temporizador de captur
  reservatorio.captura = captura.capt();              //Salva o tempo de captura
  reservatorio.captura += (reservatorio.ovf * 65535); //Soma a captura com os overflows, se houver
  reservatorio.test = 1;                              //Indica que captura foi finalizada

}//fim da deteccao da borda de descida

// === WDT resetou o MCU ===

void resetWDT()
{
  problema(3);
  delay.ms(3000);
}

#endif
