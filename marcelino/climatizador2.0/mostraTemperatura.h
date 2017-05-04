#ifndef _MOSTRA_TEMPERATURA_H
#define _MOSTRA_TEMPERATURA_H

//Funcao que mostra no display a temperaturas
void mostraTemperatura()
{
  display.set(0, 0);                      //Posiciona cursor na coluna 0 / linha 0
  display.print("TempA: ");
  display.print(temperatura.ambiente());
  display.write((uint8_t)0);              //Mostra caracter simbolo graus
  display.print("C  ");
  display.set(0, 1);                      //Posiciona cursor na coluna 0 / linha 1
  display.print("TempI: ");
  display.print(temperatura.refrigerada());
  display.write((uint8_t)0);              //Mostra caracter simbolo graus
  display.print("C  ");
}//fim da funcao mostraTemperatura

#endif
