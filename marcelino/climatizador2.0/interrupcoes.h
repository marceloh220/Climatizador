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

  uint8_t velocidade = controle.velocidade();

  // === se ventilacao ligada ===

  //se ventilacao ligada
  if (velocidade > 0) {

    //sai do modo de manutencao
    teste.clear(manutencao);

    //se em modo automatico
    if (teste.ifset(automatic)) {
      passo.automatico(300, 1400);
      posicaoEncoder = passo.passos();
    }//movimenta paletas da posicao 300 ate 1400 automaticamente

    //se posicionamento manual
    else {
      if (passo.passos() < 300)
        posicaoEncoder = 300;
      passo.posicao(posicaoEncoder);
    }
    //posiciona paletas na posicao do encoder

  }//fim do teste de motor de ventilacao ligado


  // == se em modo de manutencao ===

  //se movimentado paletas com ventilacao desligada (modo de manutencao)
  else if (teste.ifset(manutencao)) {
    //posiciona paletas
    passo.posicao(posicaoEncoder);
  }//fim do teste de modo de manutencao

  // === se ventilacao desligada e fora do modo de manutencao ===

  //se motor desligado e nao esta em modo de manutencao
  else {
    if (digital.ifset(pinfimdeCurso))
      passo.antihorario();//fecha ventilacao
    else {
      passo.parada();
      passo.passos(0);
      posicaoEncoder = 0;
    }
  }

  // === monitoramento do encoder ===

  // === simula encoder com teclas ===
  //se girando em um sentido incrementa a posicao ate 1500
  if (digital.ifclear(encoderA)) {
    teste.clear(automatic);
    teste.clear(sinaliza);
    if (posicaoEncoder < 1500)
      posicaoEncoder++;
    if (velocidade == 0)
      teste.set(manutencao);
  }
  //se girando em outro sentido decrementa a posicao ate 300 com ventilacao ligada e ate 0 com ventilacao desligada
  if (digital.ifclear(encoderB)) {
    teste.clear(automatic);
    teste.clear(sinaliza);
    if (velocidade > 0) {
      if (posicaoEncoder > 300)
        posicaoEncoder--;
    }
    else {
      teste.set(manutencao);
      if (posicaoEncoder > 0)
        posicaoEncoder--;
    }
  }
  // === fim da simulacao do encoder ===

}


// === Captura do sensor de nivel do reservatorio ===


// === WDT resetou o MCU ===

void resetWDT()
{
  erro(3);
}


// === Sleep coloca para dormir ou acorda o sistema ===

void desligamentoP() {

  teste.set(progOFF);
  external.detachINT(INT0);
  temporizacao.ms500 = timer.millis();

}

void ligamentoP() {

  sleep.disable(SLEEP);
  wdt_enable(WDTO_15MS);
  for (;;);

}



#endif
