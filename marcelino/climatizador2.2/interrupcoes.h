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
//interrupcao de overflow do timer 2
void motorPasso()
{

  //verifica a velocidade da ventilacao principal
  uint8_t velocidade = controle.velocidade();

  // === Monitor do ENCODER ===

  //guarda o estado anterior para verificar o sentido de giro do encoder
  static int16_t anterior = 3;

  //realiza a leitura dos pinos de entrada, digitalRead tem muito overhead, tive que fazer leitura direto do port pra nao ficar travando a movimentacao
  uint8_t entradas = PINB;          //le o port PINB
  entradas &= bv(PB2) | bv(PB3);    //isola os bits PB2 e PB3 (D10 e D11)
  uint8_t atual = entradas >> 2;    //acomoda os bits lidos para uma variave que verifica se o encoder esta avancando ou decrementando

  //se o ventilador estiver ligado
  if (velocidade > 0) {

    //enqanto posicao em um valor minimo e um valor maximo
    if ( encoder.limite(0,1440) ) {

      //se detectado um novo estado
      if (anterior != atual) {

        //salva em uma variavel intermediaria o valor lido,
        //em cada quadratura (um tack do encoder) esta variavel e incrementada ou decrementada 4 vezes
        encoder.encoder(atual | (anterior << 2));

        //se chegou ao final da quadratura
        if (atual == 3) {

          //salva o valor de quadratura
          encoder.atualiza();

          //quando detecta um tack do encoder vai limpar algumas flags de testes
          teste.clear(automatic);   //tira motor de passo do modo automatico
          teste.clear(sinaliza);    //desliga a sinalizacao de modo automatico
          teste.clear(manutencao);  //tira o equipamento do modo manutencao

        }//fim do teste de fim de quadratura

        //salva o valor atual no anterior para a proxima verificacao
        anterior = atual;

      }//fim do teste de novo estado

    }//fim do teste de limites

    //previne overfowls na posicao do encoder (esses overflows iriam danificar o mecanismo de movimentacao das paleras horizontais)
    if (encoder.posicao() > 1440)
      encoder.posicao(1440);
    if (encoder.posicao() < 300)//valor nao chega a zero quando a ventilacao esta ligada para as paletas ficarem meio abertas
      encoder.posicao(300);

  }//fim velocidade > 0

  //quando a ventilacao esta desligada
  else if (velocidade == 0) {

    //enquanto posicao em um valor minimo e maximo
    if ( encoder.limite(0,1440) ) {

      //se detectado um novo estado
      if (anterior != atual) {

        //salva em uma variavel intermediaria o valor lido,
        //em cada quadratura (um tack do encoder) esta variavel e incrementada ou decrementada 4 vezes
        encoder.encoder(atual | (anterior << 2));

        //se chegou ao final da quadratura
        if (atual == 3) {

          //salva o valor de quadratura
          encoder.atualiza();

          //quando detecta um tack do encoder vai limpar algumas flags de testes
          teste.clear(automatic);  //tira motor de passo do modo automatico
          teste.clear(sinaliza);   //desliga a sinalizacao de modo automatico
          teste.set(manutencao);   //coloca o equipamento do modo manutencao

        }//fim do teste de fim de quadratura

        //salva o valor atual no anterior para a proxima verificacao
        anterior = atual;

      }//fim do teste de novo estado

    }//fim do teste de limites

    //previne overfowls na posicao do encoder (esses overflows iriam danificar o mecanismo de movimentacao das paleras horizontais)
    if (encoder.posicao() > 1440)
      encoder.posicao(1440);
    if (encoder.posicao() < 0)    //com ventilacao desligada o posicionamento pode chegar a 0
      encoder.posicao(0);

  }//fim velocidade == 0

  // === Fim do monitor de encoder ===

  // === Motor de passo ===

  //se ventilacao ligada
  if (velocidade > 0) {

    //sai do modo de manutencao
    teste.clear(manutencao);

    //se em modo automatico
    if (teste.ifset(automatic)) {
      passo.automatico(300, 1400);
      encoder.posicao(passo.passos());
    }//movimenta paletas da posicao 300 ate 1400 automaticamente

    //se posicionamento manual
    else {

      //nao deixa palhetas fechar completamente com a ventilacao ligada
      if (passo.passos() < 300) {
        encoder.posicao(300);
      }
      
      passo.posicao(encoder.passo());
      
    }//posiciona paletas na posicao do encoder

  }//fim do teste de ventilacao ligado


  // == se em modo de manutencao ===

  //se movimentado paletas com ventilacao desligada (modo de manutencao)
  else if (teste.ifset(manutencao)) {
    //posiciona paletas
    passo.posicao(encoder.passo());
  }//fim do teste de modo de manutencao

  // === se ventilacao desligada e fora do modo de manutencao ===

  //se motor desligado e nao esta em modo de manutencao
  else {

    //se nao chegou no final do curso
    if (digital.ifset(pinfimdeCurso))
      passo.antihorario();//fecha ventilacao

    //quando chega no final do curso
    else {
      passo.parada();
      passo.passos(0);
      encoder.posicao(0);
    }//fim do final do curso
    
  }//fim do teste de ventilacao desligada e fora do modo de manutencao


}//fim da interrupcao motorPasso


// === Captura do sensor de nivel do reservatorio ===
//futuramente o sensor de nivel sera medido com uma captura do timer 1

// === WDT resetou o MCU ===

void resetWDT()
{
  erro(erroWDT);  //sinaliza que CPU esta travando
}


// === Sleep coloca para dormir ou acorda o sistema ===

//interrupcao para desligamento
void desligamentoProgramado()
{

  teste.set(progOFF);        //indica um desligamento programado
  external.detachINT(INT0);  //desativa a interrupcao INT0
  
  temporizacao.ms500 = timer.millis();  //faz com que o desligamento programado acontece nos proximos 500ms (debounce)

}

//interrupcao para ligamento
void ligamentoProgramado()
{

  sleep.disable(SLEEP);  //acorda a cpu
  wdt.config(RESET);     //configura o watch dog timer (WDT) para resetar o MCU
  wdt.timeout(W_16MS);   //configura o estouro do WDT no menor tempo possivel
  wdt.enable();          //habilita o WDT
  for (;;);              //fica enrolando ate a cpu resetar

}

#endif
