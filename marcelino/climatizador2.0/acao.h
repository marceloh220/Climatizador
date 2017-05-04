#ifndef _ACAO_H
#define _ACAO_H

//Funcao que realiza a acao da tecla pressionada
void acao() {

  uint8_t tecla = teclado.leitura();  //Verifica a tecla pressionada

  static uint8_t configRelogio;       //Para configuracao da data e hora

  if ( tecla == 1) {                  //Se pressionada tecla 1
    relogio.posicao(0);               //Reinicia o posicionamento de ajuste de hora
    if (mostraPTR > 0)                //Se ponteiro mostrar nao estiver na posicao 0
      mostraPTR--;                    //Decrementa ponteiro mostrar
  }

  else if ( tecla == 2) {             //Se pressionada tecla 2
    relogio.posicao(0);               //Reinicia o posicionamento de ajuste de hora
    if (mostraPTR < 3)                //Se ponteiro mostrar nao estiver na posicao 3
      mostraPTR++;                    //Incrementa ponteiro mostrar
  }

  else if ( tecla == 3) {             //Se tecla 3 pressionada
    relogio.posicao(0);               //Reinicia o posicionamento de ajuste de hora
    ventilacao.trocar();              //Troca a velocidade da ventilacao
  }

  else if ( tecla == 4)               //Se tecla 4 pressionada
    relogio.posicao();                //Posiciona cursor para ajuste de data e hora

  else if ( tecla == 5)               //Se tecla 5 pressionada
    relogio.decrementa();             //Decrementa a hora e data na posicao
    
  else if ( tecla == 6)               //Se tecla 6 pressionada
    relogio.incrementa();             //incrementa a hora e data na posicao
    
}//fim da funcao acao

#endif
