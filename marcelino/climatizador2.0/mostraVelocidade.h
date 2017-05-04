#ifndef _MOSTRA_VELOCIDADE_H
#define _MOSTRA_VELOCIDADE_H

//Funcao que mostra no display a velocidade
void mostraVelocidade() {
  display.set(0, 0);                      //Posiciona cursor na coluna 0 / linha 0
  uint8_t vel = ventilacao.velocidade();  //Verifica a velocidade
  if (vel == 0) {
    display.print("Ventilacao      ");
    display.set(0, 1);
    display.print("Desligada       ");
  }
  else if (vel == 1) {
    display.print("Velocidade      ");
    display.set(0, 1);
    display.print("Baixa           ");
  }
  else if (vel == 2) {
    display.print("Velocidade      ");
    display.set(0, 1);
    display.print("Media           ");
  }
  else if (vel == 3) {
    display.print("Velocidade      ");
    display.set(0, 1);
    display.print("Alta            ");
  }
}//fim da funcao mostraVelocidade

#endif
