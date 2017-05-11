#ifndef _DESLIGAMENTO_H_
#define _DESLIGAMENTO_H_

void desligamento() {

  wdt.clear();
  wdt.disable();
  controle.parada();
  motor.detach(OVF);

  //Inicia com a ventilacao fechada
  while (digital.read(pinfimdeCurso)) {
    passo.antihorario();
    delay.ms(4);
    if (passo.passos() < -1600)
      erro(1);
  }

  passo.parada();
  display.background(OFF);
  display.display(OFF);

  external.attach(INT0, FALLING, ligamentoP);
  sleep.disable(ALL);
  sleep.enable(SLEEP);
  sleep.sleep(POWERDOWN);

}

#endif
