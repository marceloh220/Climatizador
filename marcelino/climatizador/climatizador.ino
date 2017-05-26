/**************************************************************************************************************************

   Autor: Marcelo Henrique Moraes
   E-mail: marceloh220@hotmail.com
   Copyright (c) 2017 Marcelo Henrique Moraes

   Projeto Casa Sustentavel: Climatizador de AR
   https://github.com/marceloh220/Climatizador.git

   Codigo fonte e bibliotecas disponiveis em:
   https://github.com/marceloh220/Climatizador/tree/master/arduino

   Esquematicos do projeto em formato PDF (Apenas leitura) e DSN (simulação Proteus 7.7)
   https://github.com/marceloh220/Climatizador/tree/master/Schematics

   Projeto core Marcelino
   https://github.com/marceloh220/Marcelino.git

   Arduino IDE 1.8.2
   Marcelino Atmega328 (ATmega328/p)
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



/**************************************************************************************************************************
                                                      Bibliotecas
***************************************************************************************************************************/

//Biblioteca para controlar o display com CI PCF8574
#include <IHM8574.h>

//Biblioteca para uso do CI DS3231 ( RTC + Sensor de temperatura )
#include <DS3231.h>

/*
    Separar o codigo em varias bibliotecas aumenta a portabilidade e facilita a manutencao de um codigo.
    Eu sou adepto do "faca voce mesmo", por isso nao uso muito bibliotecas de "internet",
    o motivo disso e simples, voce como desenvolvedor precisa garantir a sua aplicacao.
    Uma biblioteca de "internet" e oferecida sem nenhuma garantia.
    Olha ali em cima:

    WITHOUT ANY WARRANTY, without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    Viu so, sem nenhuma garantia, quem tem que garantir a aplicacao e o desenvolvedor,
    e isso e feito com o estudo da biblioteca e testes.
    Algums bibliotecas sao dificeis de se entender, com pouca documentacao etc etc...
    Entao muitas vezes e mais facil criar uma biblioteca do zero e garantir o funcionamento dela,
    do que pegar uma biblioteca pronta de terceiros, quintos ou sextos,
    e essa biblioteca ocupar toda sua memoria do programa e ainda nao funcionar direito
*/


/**************************************************************************************************************************
                                                  Definicoes de Hardware
***************************************************************************************************************************/

// === Endereco TWI do CI PCF8574A (0100 A2 A1 A0 R/~W) ===

//controle dos reles
#define relayADDRESS 0b01000010

//controle do display
#define displayADDRESS 0b01000000


// === Nomes diferenciados para as coisa ===

//Deixa as coisas mais explicitas e faceis de entender
#define DESLIGADO LOW
#define LIGADO    HIGH
#define PISCANDO  TOGGLE

// === Nome para as constantes do reservatorio ===

#define nivelMIN                  100     //nivel minino do reservatorio

#define alturaReservatorio        60
#define comprimentoReservatorio   7.668711
#define larguraReservatorio       1


// === Pinos do Arduino ===

#define pinTeclado          A3     //Pino de leitura do teclado analogico
<<<<<<< HEAD
#define pinLM35             A0     //Pino de leitura do sensor de temperatura LM35
=======
#define pinLM35             A2     //Pino de leitura do sensor de temperatura LM35
>>>>>>> 425b9eef06be985639377c62f7c7827fba9ef489

#define pinUltrason          9     //Pino do sensor de volume HC-SR04, realiza o disparo do sensor ultrassonico
#define pinEcho              8     //O pino de echo do sensor ultrasonico nao pode der alterado, pois esta no hardware de captura interno do MCU

/*
   Utilizar um hardware para realizar uma tarefa ao invez de fazer por software,
   otimiza o tempo de CPU e deixa esse tempo disponivel para outras tarefas.
*/


// === Pinos do Relay ===

#define modoRele    1
//No modo 0: Reles com logica inversa, para utilizaco dos drivers como dreno de corrente (current sink)
//No modo 1: Reles com logica direta, para utilizaco dos drivers como fonte de corrente (current source)

#define velocidade1         0     //Velocidade 1 do motor de ventilacao
#define velocidade2         1     //Velocidade 2 do motor de ventilacao
#define velocidade3         2     //Velocidade 3 do motor de ventilacao
#define bombaDagua          3     //Bomda de agua do reservatorio
#define pinSinalizacao      7     //Pino de LED de sinalizacao de baixo nivel de agua do reservatorio


/*
    Quando o hardware e bem definido inicialmente, qualquer mudanca fisica fica facil,
   bastando uma unica alteracao para trocar um determinado dispositivo de pino de controle.
   Pratico e evita erros estupidos.
*/


/**************************************************************************************************************************
                                      Inicializacao dos modulos do core Marcelino
***************************************************************************************************************************/

Digital digital;        //Inicia Modulo Digital
Timer0  timer;          //Inicia Modulo do Timer0 para temporizacoes
Timer1  captura;        //Inicia Modulo do Timer1 captura de sinal
TWI     twi;            //inicia Modulo TWI no modo padrao MASTER e velocidade padrao FAST (400KHz)
Serial  serial(57600);  //Inicia Modulo Serial com baud rate de 57600
Delay   delay;          //Inicia Modulo Delay

/*
   Ao contradio do core Arduino, o core Marcelino tudo e um "modulo" que pode ou nao pode ser "inicializado".
   (Na verdade sao classes que podem ou nao ser instanciada, mas falar que sao modulos inicializados fica mais legal)
   Isso garante ao programador maior controle sobre a aplicacao, pois so precisa colocar o que realmente for utilizar
   e cada modulo e especifico para um hardware, com muitas aplicacoes e configuracoes extras.

   Minha ideia com o core Marcelino e fornecer para o desenvolvedor maior controle sobre o hardware microcontrolador
   sem perder muito da facilidade fornecida pelo Arduino.

   Claro, que com muito mais opcoes a curva de apredizado do core Marcelino aumenta, mas eu garanto que quando você
   aprende a usar um modulo voce nao vai mais conseguir largar dele.

   Estou tentando portar os modulos como bibliotecas separadas para utilizar com o core Arduino, mas no core Arduino tem muitas
   configuracoes iniciais que sao dificeis de suprimir ou transpassar, algumas dessas configuracoes sao bem inuteis diga-se de passagem,
   e algumas coisa so funcionam bem com muito malbarismo, por isso fiz um core do zero para nao ter problemas e conflitos.

   Para maiores informacoes sobre o core Marcelino consulte
   https://github.com/marceloh220/Marcelino.git
*/


/**************************************************************************************************************************
                                               Prototipo de funcoes auxiliares
***************************************************************************************************************************/

/*
   Fazer prototo das funcoes e uma boa pratica, pois permite que as funcoes sejam implementadas em qualquer local do codigo
   sem que o compilador reclame de erros, inclusive as implementacoes podem ser colocadas em outos arquivos do codigo fonte.
*/

//Funcao que mostra no display as temperaturas
void mostraTemperatura();

//Funcao que mostra no display a humidade
void mostraHumidade();

//Funcao que mostra no display a velocidade
void mostraVelocidade();

//Funcao que mostra no display o nivel de agua
void mostraNivel();

///Funcao que mostra no display a hora
void mostraHora();

//Funcao que envia o estado dos reles para a placa de controle
void enviaRele(char rele);

//Funcao de ligar  os reles
void ligaRele(char rele);

//Funcao de desligar os reles
void desligaRele(char rele);

//Funcao de alterar o estado dos reles
void trocaRele(char rele);

//Funcao que realiza a acao da tecla pressionada
void acao();

//Funcao que mede o volume de agua do reservatorio
void medirVolume();

//Cria um tipo de variavel para ponteiro de funcoes
typedef void (*funcoes)();

//Vetor de funcoes "mostrar..." para apresentar variaveis no display
//Este vetor guarda os enderecos das funcoes
<<<<<<< HEAD
func mostra[] = {mostraTemperatura, mostraHumidade, mostraVelocidade, mostraNivel, mostraHora};
=======
funcoes mostra[] = {mostraTemperatura, mostraHumidade, mostraVelocidade, mostraNivel, mostraHora};
>>>>>>> 425b9eef06be985639377c62f7c7827fba9ef489

//Ponteiro para as funcoes "mostra..."
//Basta alterar o valor do ponteiro que sera alterado a funcao que sera chamada
uint8_t mostraPTR = 0;

/*
   Guardar funcoes em um vetor e utilizar um ponteiro para definir qual funcao sera chamado
   evita o uso de um monte de "if" e "else" desnessessauros e confusos.
*/


/**************************************************************************************************************************
                                               Constantes do sistema
***************************************************************************************************************************/

//Macro de leitura de caracter salvo na memoria de programa
#define get_pgm(m,p)   pgm_read_byte(&m[p])

/*
   Macros sao de grande ajuda em um codigo, voce pode encurtar nomes muito utilizados,
   e ate colocar todo um conjunto de instrucoes repetitivas em uma macro, caso nao se queira utilizar funcoes.
*/

//Caracter simbolo de graus para display 16x2 de matriz 5x8
const uint8_t graus[8] PROGMEM =
{
  0b00000110,
  0b00001001,
  0b00001001,
  0b00000110,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000
};


/*
   O modificador PROGMEM mantem o atributo na memoria de programa,
   isso evita o uso de memoria RAM desnecessariamente para se guardar valores que nao precisarao ser alterados.
*/

/**************************************************************************************************************************
                                                  Variaveis de sistema
***************************************************************************************************************************/

//Variavel de controle dos reles
uint8_t relay;

//Variavel para configuracao do relogio
uint8_t configRelogio;

//Estrutura de dados para controle do tempo, tipo com novo nome definido
typedef struct Time {
  //Atributos por padrao sao publicos, podem ser acessados por qualquer um
  uint32_t ms10, ms60, ms500, s1;
} time_t;

//Variavel de controle do tempo criado com a estrutura Time
time_t temporizacao;

//Estrutura de dados criada com classe para controle da medicao de volume de agua do reservatorio
class Volume {
  public:                   //Atributos como publicos explicitamente
    float milimetros, centimetros, metros;  //Para distancias do sensor
    float litros, centilitros, mililitros;  //Para volumes calculados apos se saber as dimencoes do reservatorio
    uint8_t ovf;                            //Para overflow do temporizador do timer do hardware de captura
    uint64_t captura;                       //Para guardar o tempo em que o pino de echo do sensor ultrassonico permanece alto
};

//Variavel de controle de medicao de volume criada com a classe Volume
Volume volume;

/*
   No C-ANSI uma estrutura deve ser acessada de um jeito especifico, e nao existe a implementacao de classe,
   entao a primeira forma de ser declarar uma estrutura e a utilizada no C-ANSI.

   Porem no C++ existe as classes, onde e permitido a programacao orientada a objetos.
   Uma classe ajuda muito durante a proramacao, torna o codigo mais potavel
   e premite desenvolver uma aplicacao com menos codigo reescrito.
   No C++ estruturas e classes sao exatamente a mesma coisa, so mudando o nome,
   entao a segunda forma pode ser u tilizada tanto para cliacao de classes contendo apenas atributos
   ou classes com atributos e metodos.

   Utilizar estruturas e classes para guardar variaveis e uma forma de se organizar e facilitar a criacao de novas variaveis.
   A utilizacao de nomes especificos dispensa comentarios, e com as estruturas e classe um mesmo nome pode ser utilizado
   para varios dispositivos, como a temperatura do motor 1, do motor 2 ... motor 1945...

   No final todas as variaveis vao virar apenas enderecos, entao os nomes nao importam muito para os processadores.
   A organizacao e necessaria para os programadores humanos,
   entao variaveis com nomes que dizem para o que elas servem muitas vezes dispensa comentarios.
*/


/**************************************************************************************************************************
                                             Classe e structs
***************************************************************************************************************************/

//Classe de leitura de media movel
#define mediaMax     20                 //Numero de leituras analogicas a se realizar
struct Media : private Analog {         //Cria classe Media com heranca da classe Analog do core Marcelino

    /*
       com a heranca e possivel utilizar os metodos da classe da qual se herdou
       sem precisar reescrever os codigos.
    */

  private:                              //Metodos e atributos privados,
    // metodos e atributos privados so podem ser acessados pela classe, por classes e funcoes amigas e, por classes que recebem heranca
    // uma classe deve ser responsavel pelos dados que ela guarda
    // dessa forma utilizar atributos privados e permitir acesso a esses atributos atravez de metodos da propria classe
    // e uma forma de aumentar o controle e a seguranca da classe

    uint16_t analog[mediaMax];          //Vetor de leituras analogicas
    uint16_t total;                     //Somatorio das leituras analogicas
    uint16_t media;                     //Media das leituras analogicas
    uint16_t interator;                 //Interador para leituras analogicas
    uint8_t  _pin;                      //Pino de leitura analogicas

  public:                               //Metodos e atributos publico, qulquer um pode ter acesso

    Media(uint8_t pino = A0) {          //Construtor da classe Media, com parametro opcional
      //Construtores sao funcoes executadas assim que os objetos sao instanciados
      //Com o parametro opcional, se nenhum pino for passado para leitura analogica o pino A0 sera utilizado

      //limpeza do vetor de leitura analogica
      for (uint16_t i = 0; i < mediaMax; i++)
        analog[i] = 0;

      _pin = pino;                      //Configura o pino em que sera realizada a leitura analogica

    }//fim do construtor

    //Este metodo realiza um update na leitura analogica
    void update() {

      this->total -= this->analog[this->interator];         //Subtrai do total uma leitura anterior
      //'this->' deixa explicito de que o atributo ou metodo da classe atual sera utilizada

      //Realiza a leitura analogica com referencia interna de 1,1V (propriedade do metodo 'read' da classe 'Analog')

      this->analog[this->interator] = Analog::read(this->_pin, INTERNAL);

      //Utiliza um metodo da classe Analog que foi herdado
      //Utilizar uma tensao de referencia menor aumenta a resolucao, o que pode ser util

      this->total += this->analog[this->interator++];       //Soma nova leitura ao total e incrementa o interador
      this->media = this->total / mediaMax;                 //Tira a media das leituras

      if (this->interator == mediaMax)                      //Se leituras chegaram ao limite do vetor
        this->interator = 0;                                //Retorna interador para o inicio do vetor

    }//fim do metodo update

    /*
        E ideal que um metodo de uma classe seja o mais especifico possivel
        Dessa forma ele podera ser utilizado em metodos gerais
        ou utilizados pelo usuario para realizar uma tarefa especifica.
    */

    // Metodo que permite acesso ao atributo da classe, para modificar o pino de leitura analogica
    inline void pin(uint8_t pino) {
      this->_pin = pino;
    }//fim do metodo pin

    //Metodo que retorna a media das leituras analogicas
    inline uint16_t read() {
      return this->media;
    }//fim do metodo read

    /*
       Modificador 'inline' faz com que este metodo nao seja chamado, mas sim substituido durante a compilacao,
       isto e util pois reduz o atrado causado pela chamada de funcoes (call overhead),
       porem pode aumentar o tamanho ocupado pelo codigo.

       Pode ser utilizado quando o overhead precisa ser eliminado ou em metodos de poucas instrucoes.
    */

};//fim da classe media
/*
   A classe 'Media' e uma classe de uso geral e pode ser utilizada para qualquer leitura analogica,
   bastando instanciar outros objetos para utilizar os metodos em outra leitura.
   Uma caracteristica bastante util da programacao orientada a objetos,
   vou ate incluir esta classe no core Marcelino de tao util que ela e =)
*/


//Classe para organizar as leituras de temperatura
class Temperatura: private DS3231, private Media {

    /*
       Exemplo de classe geral que utliza uma classe especifica.
       Esta Classe
       Herda dos metodos  da classe DS3231 e da classe Media .
    */

  public:

    //Construtor da classe, passa para a classe Media o pino e leitura analogica do sensor LM35
    Temperatura(uint8_t pino = A0) {
      Media::pin(pino);                        //Configura pino em que sera realizada a leitura analogica
    }//fim do construtor

    //Atualiza a leitura da temperatura por media movel
    inline void update() {
      Media::update();
    }//fim do metodo update

    //Realiza a leitura da temperatura ambiente com o sensor DS3231
    inline float ambiente() {
      return DS3231::temp();
    }//fim do metodo ambiente

    //Realiza a leitura da temperatura refrigerada com o sensor LM35
    float refrigerada() {

      Media::update();                          //Atualiza a leitura analogica

      // 1.1/1023 = 0.00107526 mv (foi utilizado a tensao de referencia interna de 1,1V)
      // LM35 e um sensor de temperatura LINEAR onde 0.201mV = 20.1°C
      // portanto => 0.00107526mV = 0.107526°C

      return Media::read() * 0.107526;

    }//fim do metodo refrigerada

};//fim da classe Temperatura
/*
   Classes podem ate mesmo ser utilizadas para organizar funcoes.
*/

//classe para controle de ventilacao
class Ventilacao {

  private:
    uint8_t _velocidade;          //diz a velocidade

  public:

    //Altera a velocidade
    void velocidade(uint8_t vel) {

      //A primeira coisa a se fazer e fazer com que os reles de controle de velocidade fiquem desligados
#if modoRele == 0
      relay |= (1 << velocidade1) | (1 << velocidade2) | (1 << velocidade3);
#endif

#if modoRele == 1
      relay &= ~((1 << velocidade1) | (1 << velocidade2) | (1 << velocidade3));
#endif

      /*
         Todas as instrucoes iniciadas com # sao preprocessadores do C/C++
         Elas nao sao executadas pelo microcontrolador, mas sim pelo computador no momento da compilacao
         Esse preprocessador acima faze com que possa ser escolhido ligar os reles com logica direta ou inversa.
         Isto aumenta a facilidade na hora de montar o hardware,
         pois bastara mudar uma variavel para modificar todo o comportamento do software.
      */

      //Estrutura switch case para modificar a velocidade
      switch (vel) {

        case 1:
#if modoRele == 0
          relay &= ~(1 << velocidade1);
#endif
#if modoRele == 1
          relay |= (1 << velocidade1);
#endif
          break;

        case 2:
#if modoRele == 0
          relay &= ~(1 << velocidade2);
#endif
#if modoRele == 1
          relay |= (1 << velocidade2);
#endif
          break;

        case 3:
#if modoRele == 0
          relay &= ~(1 << velocidade3);
#endif
#if modoRele == 1
          relay |= (1 << velocidade3);
#endif
          break;

#        /*
  #           Novamente utilizado preprocessadores para determinar
  #           qual operacao logica  deve ser utilizada para se comutar os reles.
  #           A logica bit a bit permite a configuracao de um rele sem afetar o estado dos outros
  #        */

        default:
          break;
          //Se nao for enviado uma velocidade (1/2/3), saira com a ventilacao desligada

      }//fim da estrutura switch/case

      enviaRele(relay);             //envia o estado dos reles

    }//fim do metodo velocidade

    /*
       Ih, uma funcao com o mesmo nome que a outra ali encima, sera que da problema?
       Não no C++!
       O C++ diferencia as funcoes nao so pelo nome, mas tambem pela quantidade e tipo de paramentros.
       Entao a funcao foo() e diferente da funcao foo(1).
       Isso e valido apenas para compiladores C++ e nao para compiladores C,
       tudo o que se pode fazer no C pode ser feito no C++, mas o contrario nem sempre e possivel.
    */
    inline uint8_t velocidade() {
      return _velocidade;         //responde com qual velocidade em que a ventilacao esta
    }//fim do metodo velocidade

    //troca a velocidade de ventilacao, indo de desligado ate velocidade 3, voltanto para desligado
    void trocar() {
      this->_velocidade++;            //incrementa o estado da velocidade
      if (this->_velocidade == 4)     //se passou da velocidade maxima
        this->_velocidade = 0;        //coloca como desligado
      this->velocidade(_velocidade);  //chaveia os reles para nova velocidade
    }//fim do metodo trocar

};//fim da classe ventilacao


//Classe para leitura do teclado
class Teclado: private Analog {

  private:
    //variaveis de controle da classe
    uint8_t pino, tranca;

  public:

    //construtor salva em qual pino e feita a leitura analogica
    Teclado(uint8_t pin = A0) {
      pino = pin;
    }

    //realiza a leitura do teclado analogico e transforma em uma tecla
    char leitura() {

      if (this->tranca)                           //se a leitura estiver trancada
        return NULL;                              //retorna um valo nullo (0)

      //faz uma leitura analogica utilizando um metodo herdado da classe analog
      uint16_t _leitura = Analog::read(pino);

      if ( _leitura < 900)                        //se alguma tecla pressionada
        this->tranca = 1;                         //tranca a leitura do teclado

      /*
         Trancar a leitura faz surgir uma histerese util para a aplicacao.
         Apor ser feita uma leitura, apenas libera nova leitura apos a acao da tecla ter sido feita.
      */

      //verifica e retorna qual tecla foi pressionada
      if ( _leitura < 50)
        return 1;
      else if ( _leitura < 150)
        return 2;
      else if ( _leitura < 250)
        return 3;
      else if ( _leitura < 350)
        return 4;
      else if ( _leitura < 480)
        return 5;
      else if ( _leitura < 600)
        return 6;
      else if ( _leitura < 700)
        return 7;
      else if ( _leitura < 780)
        return 8;
      else if ( _leitura < 860)
        return 9;

    }//fim do metodo leitura

    //metodo que libera o teclado para nova leitura
    inline void liberar() {

      if (Analog::read(pino) > 980)             //verifica se a tecla foi solta
        this->tranca = 0;                       //se a tecla foi solta, destranca o teclado para nova leitura

    }//fim do metodo liberar

};//fim da classe teclado

/*
   As classe sao uteis para se organizar uma aplicacao, aumenta a portabilidade e facilidade de implementacao e manutencao.
   Apesar de aumentar o tamanho gasta da memoria de programa e dados,
   grandes aplicativos sao feitos com orientacao a objeto pois os beneficios sao muito maiores do que o custo.

   Todas essas classes podem ser colocadas em arquivos cabecalhos (headers.h), e importados como bibliotecas.
   Com cada classe ou conjunto de funcoes em um arquivo cabecalho especifico, a leitura do codigo fica mais facil,
   e portanto tarefas de correcao de bugs, reutilizacao, upgrades e manutecao tambem sao simplificadas.
*/


/**************************************************************************************************************************
                                              Instancias de Objetos
***************************************************************************************************************************/

//Agora com as classes criadas podemos instanciar os objetos
//(carregar os modulos, criar as variaveis chamar o zequinha etc etc etc... chame do que achar melhor)

//Display instanciado com o nome "display"
//IHM8574 NomeDoObjeto(endereco do dispositivo TWI);
IHM8574 display(displayADDRESS);

//Dispositivo RTC instanciado com o nome de relogio
<<<<<<< HEAD
DS3231 relogio;
=======
//Meses e dias da semana em pt_br
DS3231 relogio(pt_br);
>>>>>>> 425b9eef06be985639377c62f7c7827fba9ef489

//Objeto para leituras de temperatura ambiente com sensores DS3231 e de temperatura refrigerada ligada ao pino do sensor LM35
//Lembrando que o construtor da classe Temperatura recebe o pino analogico do sensor a ser lido
Temperatura temperatura(pinLM35);

//Objeto para controle da ventilacao
//Esta classe nao tem um construtor definido, o C++ cria o objeto com um construtor padrao
Ventilacao ventilacao;

//Objeto para leitura do teclado analogico
//O construtor da classe define o pino analogico do teclado
Teclado teclado(pinTeclado);


/**************************************************************************************************************************
                                                Funcoes em Interrupcoes
***************************************************************************************************************************/

/*
   Os processadores executam as instrucoes de forma sequencial, uma apos outra,
   porem existem funcoes que podem ser chamadas para execulcao tao logo um evento ocorra.
   Essas funcoes sao as interrupcoes, elas interrompem a execucao do programa
   e saltam para atender a interrupcao gerada.

   Microcontroladores AVR possuem um vetor para cada fonte de interrupcao, no caso do Atmega328
   sao 25 fontes de interrupcao que vao desde interrupcoes externas ate a leitura da memoria de programa terminada.

   Com essas interrupcoes o desenvolvedor pode colocar um hardware do MCU para trabalhar sem constante intervencao da CPU
   e quando este hardware terminar sua tarefa ele gera um sinal de interrupcao para o processador atender.

   No modulo do timer 0 do core Marcelino existe uma interrupcao implicita que e execudata a cada 1ms,
   essa interrupcao e rapida porem util, pois gera uma constante de tempo que pode ser utilizada de
   diversas maneiras na aplicação.
   Essa constante de tempo pode ser visualizada atravez do metodo millis().

   Alem da interrupcao do timer 0 para geracao de constantes de tempo,
   esta aplicacao utiliza a interrupcao do timer 1 no modo captura para
   medir a largura de um pulso externo.

   Algumas aplicacoes encontradas na internet fazem essa medicao de largura de pulso
   por software, o que prende a CPU durante a leitura, o que gasta tempo da CPU
   alem de reduzir a precisao da medicao.
   O timer 1 do MCU Atmega328 possui um hardware capaz de medir a largura de um pulso externo
   sem a necessidade de gasto do tempo da CPU, uma vez esse hardware configurado
   o processador fica livre para realizar outras tarefas.
   Quando a medicao e finalizada e gerado um sinal de interrupcao
   entao a CPU pode ser requisitada para realizar os calculos.
*/

/*
   Quando o temporizador do hardware de captura atinge o seu valor maximo,
   ele ira gerar uma interrupcao que ira jogar o processador para atender esta funcao.
*/
void medeVolumeOVF()
{
  //interrupcao de estouro do temporizador de captura (overflow)

  //Quando o timer de captura estoura, registra um overflow para somar ao tempo da captura
  volume.ovf++;

}//fim da interrupcao de overflow do temporizador de captura

/*
   Quando uma borda de subida ou descida e detectado no pino de captura ICP (pino Digital 8),
   sera gerado uma interrupcao que ira jogar o processador para atender esta funcao.

   Como as mudancas configuradas sao enviadas para a mesma funcao
   e preciso testar se foi detectado a borda de subida ou descida do sinal.
*/
void medeVolume()
{
  //interrupcao de deteccao de sinal no pino de captura

  //Se detectado a borda de subida
  if ( captura.rising() ) {

    captura.timer(CLEAR);       //Limpa o temporizador de captura
    captura.prescale(64);       //Liga o temporizador de captura com prescale 64

    //configura captura para detectar a borda de descida do sinal
    captura.attach(CAPT, FALLING, medeVolume);

  }//fim da deteccao da borda de subida

  //Se nao, quando detectado a borda de descida
  else {

    captura.prescale(OFF);      //Desliga o temporizador da captura

    //Salva o tempo de captura mais os overflows registrados
    volume.captura = captura.capt() + volume.ovf * 65535;
    //O temporizador de captura e de 16bis, portanto cada overflow equivale a 65535 ciclos

    //Limpa os overflows
    volume.ovf = 0;

    //Desanexa as funcoes de captura e overflow das interrupcoes
    captura.detach(CAPT);       //Para a interrupcao de captura (CAPT)
    captura.detach(OVF);        //Para a interrupcao de overflow (OVF)

  }//fim da deteccao da borda de descida

}//fim da interrupcao de deteccao de sinal no pino de captura


/**************************************************************************************************************************
                                                   Funcoes principais
***************************************************************************************************************************/

//Funcao de configuracao do MCU
//Umas das primeiras funcoes executadas, e chamada apenas uma vez quando o MCU e iniciado ou reiniciado
void setup()
{

  //display.create(posicao da memoria grafica, linha do simbolo, interador para salvar as oito linhas da matriz)

  //Salva caracter do simbolo de graus celcius na posicao 0 da memoria grafica do display
  for (int i = 0; i < 8; i++)
    display.create(0, get_pgm(graus, i), i);

  //Liga o background do display
  display.background(LIGADO);

  //Pino do LED como saida
  digital.mode(pinSinalizacao, OUTPUT);

  //Inicia com todos os reles desligado
#if modoRele == 0
  enviaRele(0xFF);          //Desliga todos os reles
#endif
#if modoRele == 1
  enviaRele(0x00);          //Desliga todos os reles
#endif
#  /*
  #     Preprocessadores sendo utilizados para realizar as operacoes logicas
  #    conforme o modo de operacao do controle dos reles
  #  */

}//fim da funcao setup

//Funcao para execucao do codigo em ciclo infinito.
//Fica em constante execucao enquanto o MCU estiver sendo alimentado.
void loop()
{

  //Tarefa realizada a cada 10 milisegundo
  if ( ( timer.millis() - temporizacao.ms10 ) >= 10) {    //Testa se passou 10ms

    temperatura.update();                   //Atualiza as leituras de temperatura
    acao();                                 //Chama funcao de acoes de controle
    mostra[mostraPTR]();                    //Chama funcao alocada na posicao do ponteiro mostrafuncao

    temporizacao.ms10 = timer.millis();     //Salva o tempo atual para nova tarefa apos 10ms

  }//fim da tarefa de 10ms

  //Tarefa realizada a cada 60 milisegundo
  if ( ( timer.millis() - temporizacao.ms60 ) >= 60) {    //Testa se passou 60ms

    teclado.liberar();                      //Libera o teclado para nova leitura, o tempo de 60ms garante o debounce das teclas

    temporizacao.ms60 = timer.millis();     //Salva o tempo atual para nova tarefa apos 60ms

  }//fim da tarefa de 60ms

  //Tarefa realizada a cada 500 milisegundo
  if ( ( timer.millis() - temporizacao.ms500 ) >= 500) {  //Testa se passou 500ms

    medirVolume();                          //Atualiza a leitura de volume do reservatorio

    temporizacao.ms500 = timer.millis();    //Salva o tempo atual para nova tarefa apos 500ms

  }//fim da tarefa de 500ms

  //Tarefa realizada a cada 1 segundo
  if ( ( timer.millis() - temporizacao.s1 ) >= 1000) {    //Testa se passou 1 segundo

    //Testa se o nivel de agua esta baixo
    if ( volume.mililitros < nivelMIN ) {

      if ( ventilacao.velocidade() > 0 )            //Se ventilacao esta ligada
        trocaRele(pinSinalizacao);                   //Sinaliza nivel de agua baixo no reservatorio com um led

      else                                          //Se ventilacao desligada
        desligaRele(pinSinalizacao);                //Nao incomoda ninguem com sinalizacoes desnecessarias

    }//fim do teste de nivel de agua

    temporizacao.s1 = timer.millis();       //Salva o tempo atual para nova tarefa apos 1s

  }//fim da tarefa de 1s

  /*
     Enquanto delays ocupam o processador fazendo NOP (No OPeration),
     o polling (nao e pulo) realiza verificacoes periodicas nas aplicacoes
     e testa se elas devem ser executadas.
     Utilizando a constante de tempo gerada pela interrupcao do timer 0
     o processador calcula o tempo e executa os blocos das tarefas apenas
     se o tempo determinada se passou.
     Com isso o processador fica sempre realizando alguma tarefa util, executando
     algum bloco logico, ou livre para atender prontamente a uma interrupcao.

     O polling nao e uma operacao sincrona, ela execulta o codigo sequencialmente,
     apenas eliminando a perda de tempo do delay.
     Tarefas urgentes devem preferencialmente ser colocadas em interrupcoes.
  */

}//fim da funcao loop


/**************************************************************************************************************************
                                                Funcoes auxiliares
***************************************************************************************************************************/

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
  /*
     (uint8_t) e um cast explicito.
     O cast explicito e importante para se garantir o funcionamento da aplicacao.
     Quando se conhece a biblioteca e possivel se garantir essas coisas.
  */
}//fim da funcao mostraTemperatura

//Funcao que mostra no display a humidade
void mostraHumidade() {
  display.set(0, 0);                      //Posiciona cursor na coluna 0 / linha 0
  display.print("Humidade       ");
  display.set(0, 1);                      //Posiciona cursor na coluna 0 / linha 1
  display.print("BlaBlaBla       ");      //O algoritimo do sensor de humidade ainda nao foi definidos
  //entao por enquanto e so blablabla mesmo
}//fim da funcao mostraHumidade

//Funcao que mostra no display a velocidade
void mostraVelocidade() {

  display.set(0, 0);                      //Posiciona cursor na coluna 0 / linha 0

  uint8_t vel = ventilacao.velocidade();  //Verifica a velocidade

  //Conforme a velocidade selecionada e impressa no display uma mensagem diferente
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

//Funcao que mostra no display o nivel de agua
void mostraNivel() {

  display.set(0, 0);                      //Posiciona cursor na coluna 0 / linha 0
  display.print("Nivel de Agua     ");
  display.set(0, 1);                      //Posiciona cursor na coluna 0 / linha 1
  display.print(volume.mililitros);      //Mostra nivel de agua calculado
  display.print(" mL           ");

}//fim da funcao mostraNivel

//Funcao que mostra no display a hora
void mostraHora() {
  uint8_t aux;

  display.set(0, 0);                      //Posiciona cursor na coluna 0 / linha 0
  display.print("    ");
  aux = relogio.hour();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print(':');
  aux = relogio.minute();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print(':');
  aux = relogio.second();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print("    ");
  display.set(0, 1);                      //Posiciona cursor na coluna 0 / linha 1
  display.print(relogio.weekSTR());
  display.print(' ');
  aux = relogio.day();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print(' ');
  display.print(relogio.monthSTR());
  display.print(" 20");
  aux = relogio.year();
  if (aux < 10)
    display.print('0');
  display.print(aux);
  display.print(' ');

  if (configRelogio && mostraPTR == 4) {
    display.cursor(onCURSOR);
    if (configRelogio == 1 )
      display.set(5, 0);
    else if (configRelogio == 2 )
      display.set(8, 0);
    else if (configRelogio == 3 )
      display.set(11, 0);
    else if (configRelogio == 4 )
      display.set(2, 1);
    else if (configRelogio == 5 )
      display.set(5, 1);
    else if (configRelogio == 6 )
      display.set(9, 1);
    else if (configRelogio == 7 )
      display.set(14, 1);

  }
  else
    display.cursor(noCURSOR);

}//fim da funcao mostraHora

//Funcao que envia o estado dos reles para a placa de controle
void enviaRele(char rele) {
  relay = rele;             //Atualiza estado dos reles com o valor enviado
  twi.start();              //Condicao inicial de transmissao de dados por TWI
  twi.send(relayADDRESS);   //Envia o endereco do CI PCF8574 que controla os reles
  twi.send(rele);           //Envia o estado dos reles
  twi.stop();               //Condicao de parada de transmissao de dados por TWI
}//fim da funcao enviarRele

//Funcao de ligar os reles
void ligaRele(char rele)
{
#if modoRele == 0
  cbi(relay, rele);         //Liga o rele passado
#endif
#if modoRele == 1
  sbi(relay, rele);         //Liga o rele passado
#endif
#//Preprocessadores sao legais, quando bem aplicados qualquer mudanca futura fica muito facil

  enviaRele(relay);         //Envia o estado dos reles para a placa de controle

}//fim da funcao ligarRele

//Funcao de desligar os reles
void desligaRele(char rele)
{
#if modoRele == 0
  sbi(relay, rele);         //Desliga o rele passado
#endif
#if modoRele == 1
  cbi(relay, rele);         //Desliga o rele passado
#endif
#//preprocessadores, preprocessadores, preprocessadores lalalalala

  enviaRele(relay);         //Envia o estado dos reles para a placa de controle

}//fim da funcao desligarRele

//Funcao de alterar o estado dos reles
void trocaRele(char rele)
{
  tbi(relay, rele);         //Muda o estado do rele passado
#//Aqui os preprocessadores nao sao necessarios

  enviaRele(relay);         //Envia o estado dos reles para a placa de controle

}//fim da funcao trocaRele

//Funcao que realiza a acao da tecla pressionada
void acao() {

  uint8_t tecla = teclado.leitura();  //Verifica a tecla pressionada

  if ( tecla == 1) {                  //Se pressionada tecla 1
    configRelogio = 0;
    if (mostraPTR > 0)                //Se ponteiro mostrar nao estiver na posicao 0
      mostraPTR--;                    //Decrementa ponteiro mostrar
  }

  else if ( tecla == 2) {             //Se pressionada tecla 2
    configRelogio = 0;
    if (mostraPTR < 4)                //Se ponteiro mostrar nao estiver na posicao 4
      mostraPTR++;                    //Incrementa ponteiro mostrar
  }

  else if ( tecla == 3) {               //Se tecla 3 pressionada
    configRelogio = 0;
    ventilacao.trocar();              //Troca a velocidade da ventilacao
  }

  else if ( tecla == 4) {             //Se tecla 4 pressionada
    configRelogio++;
    if (configRelogio > 7)
      configRelogio = 0;
  }

  else if ( tecla == 5) {

    if (configRelogio == 1) {
      tecla = relogio.hour();
      if (tecla > 0) {
        tecla--;
        relogio.hour(tecla);
      }
      else if (tecla == 0) {
        relogio.hour(23);
      }
    }

    if (configRelogio == 2) {
      tecla = relogio.minute();
      if (tecla > 0) {
        tecla--;
        relogio.minute(tecla);
      }
      else if (tecla == 0) {
        relogio.minute(59);
      }
    }

    if (configRelogio == 3) {
      tecla = relogio.second();
      if (tecla > 0) {
        tecla--;
        relogio.second(tecla);
      }
      else if (tecla == 0) {
        relogio.second(59);
      }
    }

    if (configRelogio == 4) {
      tecla = relogio.week();
      if (tecla > 1) {
        tecla--;
        relogio.week(tecla);
      }
      else if (tecla == 1) {
        relogio.week(7);
      }
    }

    if (configRelogio == 5) {
      tecla = relogio.day();
      if (tecla > 1) {
        tecla--;
        relogio.day(tecla);
      }
      else if (tecla == 1) {
        relogio.day(31);
      }
    }

    if (configRelogio == 6) {
      tecla = relogio.month();
      if (tecla > 1) {
        tecla--;
        relogio.month(tecla);
      }
      else if (tecla == 1) {
        relogio.month(12);
      }
    }

    if (configRelogio == 7) {
      tecla = relogio.year();
      if (tecla > 0) {
        tecla--;
        relogio.year(tecla);
      }
      else if (tecla == 0) {
        relogio.year(99);
      }
    }


  }


}//fim da funcao acao

//Funcao que inicia a medicao do volume de agua do reservatorio
void medirVolume() {

  digital.mode(pinUltrason, OUTPUT);                //pino de trigger do sensor de ultrasson configurado como saida

  captura.attach(OVF, medeVolumeOVF);               //anexa a funcao 'medeVolumeOVF' na interrupcao de overflow do temporizacao do hardware de captura
  captura.attach(CAPT, RISING, medeVolume);         //anexa a funcao 'medeVolume' na interrupcao de captura do MCU para detectar uma borda de subida do sinal no pino ICP

  digital.write(pinUltrason, LIGADO);               //liga o pulso de ultrassom do sensor HC-SR04
  delay.us(15);                                     //aguarda um tempo para que os pulsos ultrassonicos sejam enviados pelo sensor
  digital.write(pinUltrason, DESLIGADO);            //desliga o pulso de ultrassom do sensor

  //calcula as distancias do sensor ultrassonico
  // Distancia = Largura do Pulso * Velocidade do Som / 2
  // Largura do Pulso = 1/F_CPU * prescale * captura
  // Distancia = 1/F_CPU * prescale * captura * Velocidade do Som / 2
  // Distancia = captura * 1/16e6[s] * 64 * 340.29[m/s] / 2
  // Distancia = captura * 0.00068058 [m]
  volume.metros = volume.captura * 0.00068058;
  //cm = m * 10e2
  volume.centimetros = volume.captura * 0.068058;
  //mm = m * 10e3
  volume.milimetros = volume.captura * 0.68058;

  //subtrai a altura do reservatorio da distancia medida pelo sensor
  float aux = alturaReservatorio - volume.milimetros;

  //Evita valores negativos para no nivel de agua (Claro! Seria negativo porque, o reservatorio ta devendo agua pra alguem?)
  if (aux < 0)
    aux = 0;

  //Calcula o volume do reservatorio com base nas caracteristicas fisicas dele
  volume.mililitros = aux * comprimentoReservatorio * larguraReservatorio;
  volume.centilitros = volume.mililitros / 10.0;
  volume.litros = volume.mililitros / 1000.0;

  //Se o nivel de agua esta abaixo do predeterminado
  if ( volume.mililitros < nivelMIN )
    desligaRele(bombaDagua);                      //desliga a bomba d'agua

  //Se nao, se ventilacao ligada
  else if ( ventilacao.velocidade()) {
    desligaRele(pinSinalizacao);                  //desliga sinalizacao de nivel de agua baixo
    ligaRele(bombaDagua);                         //liga a bomba d'agua
  }

  //Se nao
  else {
    desligaRele(pinSinalizacao);                  //desliga sinalizacao de nivel de agua baixo
    desligaRele(bombaDagua);                      //desliga a bomba d'agua
  }

}//fim da funcao medirVolume

