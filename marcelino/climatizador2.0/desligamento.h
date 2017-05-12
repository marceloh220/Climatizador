#ifndef _DESLIGAMENTO_H_
#define _DESLIGAMENTO_H_

void desligamento() {

  DDRB |= (1 << 5);
  PORTB |= (1 << 5);

  teste.reg(0);

  controle.parada();
  display.background(OFF);
  display.display(OFF);

  wdt.disable();
  sleep.disable(ALL);

  //Inicia com a ventilacao fechada
  while (digital.read(pinfimdeCurso)) {
    passo.antihorario();
    delay.ms(4);
    if (passo.passos() < -1600)
      erro(1);
  }

  passo.desligar();

  external.attach(INT0, FALLING, ligamentoP);
  sleep.sleep(POWERDOWN);
  for (;;);

}

#endif
