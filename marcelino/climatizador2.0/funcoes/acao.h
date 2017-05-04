#ifndef _ACAO_H
#define _ACAO_H

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
void acao() {

  uint8_t tecla = teclado.leitura();  //Verifica a tecla pressionada
  uint8_t elem = tamVet(mostra)-1;    //verifica a quantidade de elementos no vetor mostra

  if ( tecla == 1 ) {                 //Se pressionada tecla 1
    relogio.posicao(0);               //Reinicia o posicionamento de ajuste de hora
    if (mostraPTR > 0)                //Se ponteiro mostrar nao estiver no primeiro elemento
      mostraPTR--;                    //Decrementa ponteiro mostrar
  }

  else if ( tecla == 2 ) {            //Se pressionada tecla 2
    relogio.posicao(0);               //Reinicia o posicionamento de ajuste de hora
    if (mostraPTR < elem) 		  	  //Se ponteiro mostrar nao estiver no ultimo elemento
      mostraPTR++;                    //Incrementa ponteiro mostrar
  }

  else if ( tecla == 3 ) {            //Se tecla 3 pressionada
    relogio.posicao(0);               //Reinicia o posicionamento de ajuste de hora
    ventilacao.trocar();              //Troca a velocidade da ventilacao
  }

  else if ( tecla == 4 ) {            //Se tecla 4 pressionada
    if( ventilacao.velocidade() > 0)  //Se ventilacao estiver ligada
		rele.troca(direcaoVertical);  //Liga ou desliga as palhetas verticais de direcionamento de ventilacao
	else 							  //Se nao
		rele.desliga(direcaoVertical);//Desliga as palhetas verticais de direcionamento de ventilacao
  }
  
  else if ( tecla == 5 )              //Se tecla 5 pressionada
	if( mostraPTR == elem)			  //Se menu estiver na ultima posicao
		relogio.posicao();            //Avanca o posicionamento do cursor para ajuste de data e hora

  else if ( tecla == 6 )              //Se tecla 6 pressionada
	if( relogio.ajuste() )			  //Se relogio estiver em posicao de ajuste
		relogio.decrementa();         //Decrementa a hora e data na posicao

  else if ( tecla == 7)               //Se tecla 7 pressionada
  	if( relogio.ajuste() )			  //Se relogio estiver em posicao de ajuste
		relogio.incrementa();         //incrementa a hora e data na posicao

}//fim da funcao acao

#endif
