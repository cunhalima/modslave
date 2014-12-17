#
# The MIT License (MIT)
#
# Copyright (c) 2014 Alex R. Cunha Lima, Dinara Rigon, Gabrielle A. de Souza
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

# VER: http://www.singularity.be/2009/07/pic16f627a-with-rs232-and-sdcc.html
#
#PIC=16f628a
#PIC=16f627a
PIC=18f4550
PRG=slave
CFILES=main.c lib.c
HEX=$(PRG).hex
COD=$(PRG).cod
EXTRA=$(PRG).lst $(PRG).map
OBJFILES=$(patsubst %.c,%.o,$(CFILES))
LSTFILES=$(patsubst %.c,%.lst,$(CFILES))
ASMFILES=$(patsubst %.c,%.asm,$(CFILES))
DIST=trab.tar.gz
GPSIM=gpsim
GPASM=gpasm
GPLINK=gplink
GPLDFLAGS=-w -m
GPLDFLAGS+=-s /usr/share/gputils/lkr/$(PIC).lkr
GPLDFLAGS+=$(HOME)/sdcc/share/sdcc/non-free/lib/pic16/pic$(PIC).lib
GPLDFLAGS+=$(HOME)/sdcc/share/sdcc/lib/pic16/libsdcc.lib
SDCC=sdcc
#SDCCFLAGS=-S -mpic16 -p$(PIC) --use-non-free -D__$(PIC) -I/home/alex/sdcc/share/sdcc/include/pic16
SDCCFLAGS=-S -mpic16 -p$(PIC) --use-non-free -D__$(PIC)

.SUFFIXES:
SUFFIXES :=
.c.o:

.PHONY: all clean run dist crun test

.PRECIOUS: %.asm

all: $(HEX)

dist: $(DIST)

clean:
	@rm -f $(HEX) $(COD) $(EXTRA) $(OBJFILES) $(LSTFILES) $(ASMFILES)

run: $(HEX)
	@$(GPSIM) -c env.conf -s $(COD)

crun: $(HEX)
	@$(GPSIM) --cli -c env.conf -s $(COD)

$(HEX): $(OBJFILES)
	@$(GPLINK) $(GPLDFLAGS) -o $@ $^

%.o: %.asm
	@$(GPASM) -w 1 -c $<

%.asm: %.c defs.h lib.h
	@$(SDCC) $(SDCCFLAGS) $<

$(DIST): $(HEX)
	@tar -cvzf $(DIST) Makefile $(HEX) $(SRC)

upload:
	k14 p $(HEX)
	k14 v $(HEX)

mt: main.c
	@gcc -DTEST main.c -o mt

test: mt
	@./mt
