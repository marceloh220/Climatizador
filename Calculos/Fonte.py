'''

Calculos para o climatizador.

Python 2 ou superior


'''
import os
import math

'''

Filtro de linha para corrente de 10 amperes

'''
print("Filtro de linha:")
Vrede = 127			#tensao da rede
Frede = 60			#frequencia da rede
CorrenteRede = 10	#corrente maxima do filtro
omega = 2 * math.pi * Frede #2pi*f
Vrede_pico = Vrede * math.sqrt(2) 	#tensao de pico da rede

print("	Tensao de rede:" + str(Vrede) + "v")
print("	Frequencia de rede:" + str(Frede) + "H")
print("	Corrente do filtro:" + str(CorrenteRede) + "A")
print("	Tensao de pico da rede: " + str(Vrede_pico) + "v" )

Rrede = Vrede / CorrenteRede			#resistencia do filtro

print("	Resistencia equivalente para a rede: " + str(Rrede) + "ohm" )

# Xl = omega * L
# L = Xl / omega
LChoque = Rrede / omega		#calculo da indutacia total do filtro choque

print("	Indutor do Choque: " + str(LChoque) + "H" )

#corte da 5 harmonica
Impar = Frede * 5		   #filtro corta a partir da 5 harmonica

# 3% de harmonica
# Corte = (h - 3%)
HCorte = Impar - Impar * (3/100)

#filtro passa baixa
# Fcorte = 1 / ( omega * raiz2( L * C) )
# raiz2(C) = 1 / ( Fcorte * raiz2(L) * omega )
# C = ( 1 / ( Fcorte * raiz2(L) * omega ) )^2
CChoque = ( 1 / (HCorte * math.sqrt(LChoque) * omega) )**2	#calculo do capacitor para o filtro
print("	Capacitor do Choque: " + str(CChoque) + "F" )
#Fcorte : Frequencia de corte
#omega: 2pi*frequencia
#raiz2: raiz quadrada
#L: indutancia
#C: capacitancia

'''

Filtro de ripple para fonte 12V 600mA

'''
print("Fonte de 12V:")
Vfonte = 12							#tensao rms do transformador
CorrenteFonte = 0.8					#corrente fornecida pela fonte
Vfonte_p = Vfonte * math.sqrt(2)	#tensao de pico da saida do transformador
omega_fonte = 2 * math.pi * 120		#omega da fonte
Cfonte = 2000						#Capacitor de filtro da fonte
Ffonte = Frede * 2					#depois de passar pelo retificador de onda completa a frequencia dobra
omega_fonte = 2 * math.pi * Ffonte	#omega da fonte
print("	Tensao de fonte:" + str(Vfonte) + "v")
print("	Corrente de fonte:" + str(CorrenteFonte) + "A")
print("	Tensao de pico da fonte: " + str(Vfonte_p) + "v" )

Rfonte = Vfonte / CorrenteFonte		#resistencia maxima da fonte

print("	Resistencia equivalente para a fonte: " + str(Rfonte) + "ohm" )

#rippe = I / (fC)
Ripple_semfiltro = CorrenteFonte / (Ffonte*Cfonte)	#calculo de tensao de ripple
print("	Tensao ripple sem filtro pi:" + str(Ripple_semfiltro) + "v")

print("Filtro pi:")
Lfonte = Rfonte / omega

print("	Indutor do fonte: " + str(Lfonte) + "H" )

C1 = 100e-9
C2 = 100e-9

Ripple_confiltro = Vfonte * (1 / omega_fonte**3 * C1 * C2 * Lfonte * Rfonte)

print("	Ripple da fonte com filtro pi: " + str(Ripple_confiltro) + "v" )

'''

Regulador 12V para servo motor
LM317

Vout = 1.25 * ( 1 + R2 /  R1 )

R2 = R1 * (Vout - 1.25) / 1.25

'''
print("Regulador de 12V:")
R1_12 = 330
Vout_12 = 12

R2_12 = R1_12 * (Vout_12 - 1.25) / 1.25

print("	R2 regulador de 12V: " + str(R2_12) + "ohm" )

'''

Regulador 5V para circuitos logicos
LM317

Vout = 1.25 * ( 1 + R2 /  R1 )

R2 = R1 * (Vout - 1.25) / 1.25

'''
print("Regulador de 5V:")
R1_5 = 330
Vout_5 = 5

R2_5 = R1_5 * (Vout_5 - 1.25) / 1.25

print("	R2 regulador de 5V: " + str(R2_5) + "ohm" )

os.system("pause")
