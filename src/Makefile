# Valid parameters for mcu:
#   at90s2323
#   at90s8515
#   at90s8535
#   atmega161

#ARCH=at90s2323
ARCH=at90s8515
#ARCH=at90s8535
#ARCH=atmega161

CC=avr-gcc -mmcu=$(ARCH)
AS=avr-as
OBJCOPY=avr-objcopy
SIZE=avr-size

CFLAGS=-Wall -I. -Os -mcall-prologues
ASFLAGS=-Wall -I.

CSRC= \
	hal.c \
	main.c \
	sw.c \
	t0.c \
	tools.c \

SSRC= \
	eepromi2c.S \
	io.S \

OBJ=$(CSRC:.c=.o) $(SSRC:.S=.o)

SLOCFILES =  config.h eepromi2c.S sw.c \
	sw.h hal.c hal.h io.S main.c main.h t0.c \
	t0.h tools.c tools.h types.h \

CALLTREEFILES = sw.c hal.c main.c t0.c tools.c 

all: sosse eedata

sosse: $(OBJ)
	$(CC) $(CFLAGS) -Wl,-Map,sosse.map -o sosse $(OBJ)
	$(SIZE) sosse
	$(OBJCOPY) -O binary sosse sosse.bin
	$(OBJCOPY) -O ihex sosse sosse.hex

eedata:
	$(CC) $(ASFLAGS) -c -o eedata -DDAY=0x00 -DMONTH=0x00 -DYEAR=0x00 eedata.S
	$(OBJCOPY) -O binary eedata eedata.bin
	$(OBJCOPY) -O ihex eedata eedata.hex

sloc:
	@echo
	@echo Lines of code
	@sloccount $(SLOCFILES) | grep -A 1000 "SLOC-by-Language" | grep -v "^$$"

calltree:
	@echo
	@echo Calltree
	@/opt/schily/bin/calltree -m -I. -I/opt/avrgcc/avr/include/ -D__AVR_AT90S8515__ $(CALLTREEFILES)

calldepth:
	@echo
	@echo Calldepth
	@/opt/schily/bin/calltree -m -I. -I/opt/avrgcc/avr/include/ -D__AVR_AT90S8515__ $(CALLTREEFILES) | sed -e "s/[^ ].*$$//" -e "s/    /#/g" -e "s/#/1/" -e "s/#/2/" -e "s/#/3/" -e "s/#/4/" -e "s/#/5/" -e "s/#/6/" -e "s/#/7/" -e "s/#/8/" -e "s/#/9/" -e "s/#/0/" | sort | tail -1

uncalled:
	@echo
	@echo Uncalled
	@/opt/schily/bin/calltree -u -I. -I/opt/avrgcc/avr/include/ -D__AVR_AT90S8515__ $(CALLTREEFILES)

statistics: sloc calldepth uncalled

clean: clean-sosse clean-eedata
	rm -rf core

clean-sosse:
	rm -rf $(OBJ) sosse sosse.bin sosse.hex sosse.map

clean-eedata:
	rm -rf eedata eedata.bin eedata.hex eedata.lst

