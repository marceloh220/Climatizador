Bibliotecas desenvolvidas para o climatizador de Ar com core Marcelino

Autor: Marcelo Henrique Moraes
E-mail: marceloh220@hotmail.com
Copyright (c) 2017 Marcelo Henrique Moraes

Projeto Casa Sustentavel: Climatizador de AR
https://github.com/marceloh220/Climatizador.git

Codigo fonte e bibliotecas disponiveis em:
https://github.com/marceloh220/Climatizador/tree/master/marcelino

Esquematicos do projeto em formato PDF (Apenas leitura) e DSN (simulação Proteus 7.7)
https://github.com/marceloh220/Climatizador/tree/master/Schematics

Projeto core Marcelino
https://github.com/marceloh220/Marcelino.git

Arduino IDE 1.8.2
Marcelino Atmega328 (ATmega328/p)
F_CPU 16MHz (Cristal externo)

Instalação do core Marcelino:

copiar a pasta marcelino para a pasta hardware no diretorio de instalacao do Arudino IDE

windows 7 32bits
marcelino >> C:\Program Files\Arduino\hardware
windows 7 64bits
marcelino >> C:\Program Files (x86)\Arduino\hardware

Apos instalado, reinicie a IDE.

Em Ferramentas/Placa ira aparecer duas novas placas:
Marcelino Atmega328 (Arduino Uno/Duemilanove/Nano ou qualquer outra coisa com MCU Atmega328/p)
Marcelino Atmega168 (Arduino Diecimila ou qualquer outra coisa com MCU Atmega168a/pa)

Em F_CPU e selecionado a velocidade:
F_CPU 16MHz (cristal externo de 16MHz)
F_CPU 8MHz	(	oscilador interno,
				dispensa cristal externo,
				dois pinos digitais extras 9 (D20) e 10 (D21),
				possibilidade de uso do timer 2 em modo assincrono com cristal nos pinos 9 (x1) e 10 (x2)
			 )
