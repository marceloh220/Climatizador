#ifndef _ACIONAMENTOS_H
#define _ACIONAMENTOS_H

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

//Funcao que realiza a acao da tecla pressionada
void acionamentos() {

  uint8_t tecla = teclado.leitura();  //Verifica a tecla pressionada
  uint8_t elem = tamVet(mostra) - 1;	//verifica a quantidade de elementos no vetor

  //  serial.print("Tecla pressionada: ");
  //  serial.println(tecla);

  if (tecla != 0) {        //Se alguma tecla pressionada
    teste.set(acteclado);  //Indica acao do teclado
    teste.clear(progOFF);  //Desativa a flag de desligamento programado
  }

  if ( tecla == 1 ) {   //Se pressionada tecla 1
    relogio.posicao(0); //Reinicia o posicionamento de ajuste de hora
    if (mostraPTR > 0)  //Se ponteiro mostrar nao estiver no primeiro elemento
      mostraPTR--;      //Decrementa ponteiro mostrar
  }//fim teste tecla 1

  else if ( tecla == 2 ) {  //Se pressionada tecla 2
    relogio.posicao(0);     //Reinicia o posicionamento de ajuste de hora
    if (mostraPTR < elem) 	//Se ponteiro mostrar nao estiver no ultimo elemento
      mostraPTR++;          //Incrementa ponteiro mostrar
  }//fim teste tecla 2

  else if ( tecla == 3 ) {        //Se tecla 3 pressionada
    mostraPTR = 1;                //Posiciona o menu para apresentar o estado das velocidades
    relogio.posicao(0);           //Reinicia o posicionamento de ajuste de hora
    controle.trocar(VELOCIDADE);  //Troca a velocidade da controle (desligado/1/2/3)
  }//fim teste tecla 3

  else if ( tecla == 4 ) {      //Se tecla 4 pressionada
    relogio.posicao(0);         //Reinicia o posicionamento de ajuste de hora
    controle.trocar(VERTICAL);  //Liga ou desliga a movimentacao das paletas verticais de direcionamento de ar
  }//fim teste tecla 4

  else if ( tecla == 5 ) {  //Se tecla 5 pressionada
    mostraPTR = elem;       //Posiciona o menu para apresentar as horas com configuracoes
    if ( mostraPTR == elem) //Se menu estiver na ultima posicao
      relogio.ajuste();     //Avanca o posicionamento do cursor para ajuste de hora/data
  }//fim teste tecla 5

  else if ( tecla == 6 ) {    //Se tecla 6 pressionada
    if ( relogio.posicao() )  //Se relogio estiver em posicao de ajuste
      relogio.decrementa();   //Decrementa a hora/data na posicao
  }//fim teste tecla 6

  else if ( tecla == 7) {     //Se tecla 7 pressionada
    if ( relogio.posicao() )  //Se relogio estiver em posicao de ajuste
      relogio.incrementa();   //incrementa a hora/data na posicao
  }//fim teste tecla 7

  // === botao do encoder ===

  if (teste.ifclear(travaEnco)) {           //Verifica se o botao do encoder esta liberado para leitura

    if (controle.velocidade() > 0) {        //Se ventilacao ligada

      if (digital.ifclear(encoderButton)) { //Se botao do encoder pressionado
        teste.set(travaEnco);               //Trava a leiruta do botao do encoder
        teste.toggle(automatic);            //Liga ou desliga o movimento automatico do motor de passo
        teste.toggle(sinaliza);             //Liga ou desliga sinalizacao de automatico
      }//fim do teste do botao

    }//fim do teste de velocidade

    else {                    //Se ventilacao desligada
      teste.clear(automatic); //Tira movimentacao do modo automatico
      teste.clear(sinaliza);  //Desliga a sinalizacao de automatico

      //Se botao do encoder pressionado
      if (digital.ifclear(encoderButton)) {

        teste.set(travaEnco);  //Trava a leiruta do botao do encoder
        fechamento();          //Realiza o fechamento das paletas de ventilacao frontais

      }//fim do teste de botao do encoder

    }//fim do teste de ventilacao desligada

  }//fim do teste de trava do botao do encoder

  // === gerador de Anion ===
  if (controle.velocidade() > 0)
    controle.reles(geradorAnion, HIGH);  //Se ventilacao principal ligada, liga gerador de anion
  else
    controle.reles(geradorAnion, LOW);  //Se ventilacao principal desligada, desliga o gerador de anion


}//fim da funcao acao

#endif
