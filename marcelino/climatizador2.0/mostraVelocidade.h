#ifndef _MOSTRA_VELOCIDADE_H
#define _MOSTRA_VELOCIDADE_H

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

//Funcao que mostra no display a velocidade
void mostraVelocidade() {

  display.set(0, 0);						          //Posiciona cursor na coluna 0 / linha 0
  display.print("Ventilacao");
  
  if (teste.ifset(automatic))
    display.print("  auto");
  else if (teste.ifset(manutencao))
    display.print("  mntc");
  else
    display.print("      ");
    
  uint8_t vel = controle.velocidade();	  //Verifica a velocidade

  display.set(0, 1);

  if (vel == 0)
    display.print("Desligada       ");
  else if (vel == 1)
    display.print("Baixa           ");
  else if (vel == 2)
    display.print("Media           ");
  else if (vel == 3)
    display.print("Alta            ");

}//fim da funcao mostraVelocidade

#endif
