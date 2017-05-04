#ifndef _MOSTRA_NIVEL_H
#define _MOSTRA_NIVEL_H

//Funcao que mostra no display o nivel de agua
void mostraNivel() {
  display.set(0, 0);                      //Posiciona cursor na coluna 0 / linha 0
  display.print("Nivel de Agua     ");
  display.set(0, 1);                      //Posiciona cursor na coluna 0 / linha 1
  display.print(volume.mililitros);      //Mostra nivel de agua calculado
  display.print(" mL           ");
}//fim da funcao mostraNivel

#endif
