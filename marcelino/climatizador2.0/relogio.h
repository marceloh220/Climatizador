#ifndef _RELOGIO_H
#define _RELOGIO_H

#include <DS3231.h>

class Relogio: private DS3231 {
  private:
    uint8_t _ajuste;
  public:

    Relogio(uint8_t lng) {
      DS3231::language(lng);
    }

    void posicao() {
      _ajuste++;
      if (_ajuste > 7)
        _ajuste = 0;
    }

    void posicao(uint8_t pos) {
      _ajuste = pos;
    }

    uint8_t ajuste() {
      return _ajuste;
    }
    
    void decrementa() {						  //decrementa as datas
      int8_t aux;
      switch (_ajuste) {					  //Testa as posicoes de ajuste
        case 1:							        //Se cursor posicionado para ajuste de hora
          aux = DS3231::hour();     //Le hora do relogio RTC
          aux--;                    //Decrementa hora
          if (aux < 0)              //Se hora passou de 0
            aux = 23;               //Retorna hora para 23
          DS3231::hour(aux);        //Salva hora ajustada
          break;
        case 2:							        //Se cursor posicionado para ajuste de minuto
          aux = DS3231::minute();   //Le minuto do relogio RTC
          aux--;                    //Decrementa minuto
          if (aux < 0)              //Se minuto passou de 0
            aux = 59;               //Retorna minuto para 59
          DS3231::minute(aux);      //Salva minuto ajustado
          break;
        case 3:							        //Se cursor posicionado para ajuste de hora
          aux = DS3231::second();   //Le segundo do relogio RTC
          aux--;                    //Decrementa segundo
          if (aux < 0)              //Se segundo passou de 0
            aux = 59;               //Retorna segundo para 59
          DS3231::second(aux);      //Salva segundo ajustado
          break;
        case 4:							        //Se cursor posicionado para ajuste de semana
          aux = DS3231::week();     //Le semana do relogio RTC
          aux--;                    //Decrementa semana
          if (aux < 1)              //Se semana for menor que 1 (domingo)
            aux = 7;                //Retorna semana para 7 (sabado)
          DS3231::week(aux);        //Salva semana ajustado
          break;
        case 5:							        //Se cursor posicionado para ajuste de dia
          aux = DS3231::day();     	//Le dia do relogio RTC
          aux--;                    //Decrementa dia
          if (aux == 0)             //Se dia for menor que 1
            aux = 31;               //Retorna dia para 31
          DS3231::day(aux);         //Salva dia ajustado
          break;
        case 6:							        //Se cursor posicionado para ajuste de mes
          aux = DS3231::month();    //Le mes do relogio RTC
          aux--;                    //Decrementa mes
          if (aux == 0)             //Se mes for 0
            aux = 12;               //Retorna mes para 12
          DS3231::month(aux);       //Salva mes ajustado
          break;
        case 7:							        //Se cursor posicionado para ajuste de ano
          aux = DS3231::year();     //Le ano do relogio RTC
          aux--;                    //Decrementa ano
          if (aux < 0)              //Se ano passou de 0
            aux = 99;               //Retorna ano para 99
          DS3231::year(aux);        //Salva ano ajustado
          break;
        default:						        //Se cursor em qualquer outra posicao
          break;						        //Fazer nada
      }//fim do switch

    }//fim de decremento

    void incrementa() {						  //incrementa as datas
      int8_t aux;
      switch (_ajuste) {					  //Testa as posicoes de ajuste
        case 1:							        //Se cursor posicionado para ajuste de hora
          aux = DS3231::hour();     //Le hora do relogio RTC
          aux++;                    //Incrementa hora
          if (aux == 24)            //Se hora chegou em 24
            aux = 0;                //Retorna hora para 0
          DS3231::hour(aux);        //Salva hora ajustada
          break;
        case 2:							        //Se cursor posicionado para ajuste de minuto
          aux = DS3231::minute();   //Le minuto do relogio RTC
          aux++;                    //Incrementa minuto
          if (aux == 60)            //Se minuto chegou em 60
            aux = 0;                //Retorna minuto para 0
          DS3231::minute(aux);      //Salva minuto ajustado
          break;
        case 3:							        //Se cursor posicionado para ajuste de hora
          aux = DS3231::second();   //Le segundo do relogio RTC
          aux++;                    //Incrementa segundo
          if (aux == 60)            //Se segundo chegou em 60
            aux = 0;                //Retorna segundo para 0
          DS3231::second(aux);      //Salva segundo ajustado
          break;
        case 4:							        //Se cursor posicionado para ajuste de semana
          aux = DS3231::week();     //Le semana do relogio RTC
          aux++;                    //Incrementa semana
          if (aux > 7)              //Se semana passou de 7 (sabado)
            aux = 1;                //Retorna semana para 1 (domingo)
          DS3231::week(aux);        //Salva semana ajustado
          break;
        case 5:							        //Se cursor posicionado para ajuste de dia
          aux = DS3231::day();     	//Le dia do relogio RTC
          aux++;                    //Incrementa dia
          if (aux > 31)             //Se dia maior que 31
            aux = 1;                //Retorna dia para 1
          DS3231::day(aux);         //Salva dia ajustado
          break;
        case 6:							        //Se cursor posicionado para ajuste de mes
          aux = DS3231::month();    //Le mes do relogio RTC
          aux++;                    //Incrementa mes
          if (aux > 12)             //Se mes passou de 12(dezembro)
            aux = 1;                //Retorna mes para 1(janeiro)
          DS3231::month(aux);       //Salva mes ajustado
          break;
        case 7:							        //Se cursor posicionado para ajuste de ano
          aux = DS3231::year();     //Le ano do relogio RTC
          aux++;                    //Incrementa ano
          if (aux > 99)             //Se ano passou de 99
            aux = 0;                //Retorna ano para 0
          DS3231::year(aux);        //Salva ano ajustado
          break;
        default:						        //Se cursor em qualquer outra posicao
          break;						        //Fazer nada
      }//fim do switch
    }//fim de incremento

    //Retorna datas e horarios
    inline uint8_t hora() {return DS3231::hour();}
    inline uint8_t minuto() {return DS3231::minute();}
    inline uint8_t segundo() {return DS3231::second();}
    inline char* semana() {return DS3231::weekSTR();}
    inline uint8_t dia() {return DS3231::day();}
    inline char* mes() {return DS3231::monthSTR();}
    inline uint8_t ano() {return DS3231::year();}

};


#endif
