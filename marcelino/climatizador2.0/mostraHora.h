#ifndef _MOSTRA_HORA_H
#define _MOSTRA_HORA_H

//Funcao que mostra no display a hora
void mostraHora() {
  uint8_t aux;
  display.set(0, 0);                      //Posiciona cursor na coluna 0 / linha 0
  display.print("    ");
  aux = relogio.hora();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print(':');
  aux = relogio.minuto();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print(':');
  aux = relogio.segundo();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print("    ");
  display.set(0, 1);                      //Posiciona cursor na coluna 0 / linha 1
  display.print(relogio.semana());
  display.print(' ');
  aux = relogio.dia();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print(' ');
  display.print(relogio.mes());
  display.print(' ');
  aux = relogio.ano();
  display.print(2000+aux);    delay.ms(30);
  display.print(' ');
  if (relogio.ajuste() && mostraPTR == 3) {
    display.cursor(onCURSOR);
    if (relogio.ajuste() == 1 )
      display.set(5, 0);
    else if (relogio.ajuste() == 2 )
      display.set(8, 0);
    else if (relogio.ajuste() == 3 )
      display.set(11, 0);
    else if (relogio.ajuste() == 4 )
      display.set(2, 1);
    else if (relogio.ajuste() == 5 )
      display.set(5, 1);
    else if (relogio.ajuste() == 6 )
      display.set(9, 1);
    else if (relogio.ajuste() == 7 )
      display.set(14, 1);
  }
  else
    display.cursor(noCURSOR);
}//fim da funcao mostraHora

#endif
