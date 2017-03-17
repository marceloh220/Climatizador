#Marcelo H Moraes
#marceloh220@hotmail.com
#
#Used with avr
#Dependences: avr-gcc, avr-binutils
#
#
#The MIT License (MIT)
#
#Copyright (c) 2017 Marcelo Henrique Moraes Dermones
#
#Permission is hereby granted, free of charge, to any person obtaining a copy of
#this software and associated documentation files (the "Software"), to deal in
#the Software without restriction, including without limitation the rights to
#use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
#the Software, and to permit persons to whom the Software is furnished to do so,
#subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all
#copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
#FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
#COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
#IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
#CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#options of sketch
sketch = climatizador.cpp
mcu = atmega328p
clock = 8000000UL

#programer
downmcu = m328
port = /dev/ttyUSB0
programmer= usbasp
#downspeed = 115200
downspeed = 57600

#options of compiler
#debug = -g -ffunction-sections -fdata-sections
debug = -g2 -gdwarf-2
warnings = -Wall
optmization = -Os -MMD -flto -felide-constructors
CPPflags = -std=gnu++11 -fpermissive -fno-threadsafe-statics -fno-exceptions -x c++
LINKflags = -fuse-linker-plugin -Wl,--gc-sections

#binaries
compilerCPP = tools/avr-gcc/bin/avr-g++
compilerC = tools/avr-gcc/bin/avr-gcc
link = tools/avr-gcc/bin/avr-gcc-ar
pack = tools/avr-gcc/bin/avr-objcopy
uploader = tools/avrdude/avrdude
uploaderconf = tools/avrdude/avrdude.conf

all:
	
	$(compilerCPP) $(optmization) $(CPPflags) $(debug) $(warnings) -mmcu=$(mcu) -DF_CPU=$(clock) "-Icore/" -c core/*.cpp
	$(compilerC) -c -g -x assembler-with-cpp -flto -mmcu=$(mcu) -DF_CPU=$(clock) "-Icore/" -c core/*.S
	
	mkdir -p object/ && mv *.o object/
	
	$(link) rcs core.a -o object/*.o

	mv core.a object/
	
	$(compilerCPP) $(optmization) $(CPPflags) $(debug) $(warnings) -mmcu=$(mcu) -DF_CPU=$(clock) "-Icore/" "-Ilibrary/" -c $(sketch) -o $(sketch).o
	
	$(compilerCPP) $(optmization) $(LINKflags) $(debug) $(warnings) -mmcu=$(mcu) $(sketch).o object/core.a -o $(sketch).elf
	
	$(pack) -O ihex $(sketch).elf $(sketch).hex
	
	avr-size -C  -x --mcu=$(mcu) $(sketch).elf

	rm -f *.o
	rm -f *.d

link:
	$(compilerCPP) $(optmization) $(CPPflags) $(debug) $(warnings) -mmcu=$(mcu) -DF_CPU=$(clock) "-Icore/" "-Ilibrary/" -c $(sketch) -o $(sketch).o
	
	$(compilerCPP) $(optmization) $(LINKflags) $(debug) $(warnings) -mmcu=$(mcu) $(sketch).o object/core.a -o $(sketch).elf
	
	$(pack) -O ihex $(sketch).elf $(sketch).hex
	
	avr-size -C  -x --mcu=$(mcu) $(sketch).elf
	
	rm -f *.o
	rm -f *.d
	
	
download:
	$(uploader) -C $(uploaderconf) -c $(programmer) -p $(downmcu) -b $(downspeed) -P $(port) -U flash:w:"$(sketch).hex":a
	
clean:
	rm -rf object/
	rm -f *.o
	rm -f *.elf
	rm -f *.hex
	rm -f *.d
	rm -r *.a
