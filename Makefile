SRCS=$(wildcard *.c)
HDRS=$(wildcard *.h)
WORDS=$(wildcard builtins/*.nf)
CC=gcc -std=c99 -Wall -Werror -g -O0 -fno-optimize-sibling-calls

all: notforth

test: notforth
	./notforth TEST

debug: notforth
	gdb -ex run notforth

run: notforth
	./notforth

clean:
	> builtins/all.c
	rm -f notforth nf-bootstrap builtins/*.nf.impl *.o notforth.elf notforth.hex

notforth: ${SRCS} ${HDRS} builtins/all.c
	${CC} -DLINUX -o notforth ${SRCS}


# builtins/all.c: $(WORDS:%.nf=%.nf.impl)
# builtins/all.h: $(WORDS:%.nf=%.nf.dict)
#builtins/%.nf.dict builtins/%.nf.impl: builtins/%.nf nf-bootstrap builtins.sh
builtins/all.c: nf-bootstrap builtins.sh builtins/LIST ${WORDS}
	bash builtins.sh

# This secretly depends on builtins/all.{h,c}, in that it incorporates them
# into the build, but it does not *require* them, and including that dependency
# explictly would make the build graph cyclical.
nf-bootstrap: ${SRCS} ${HDRS}
	touch builtins/all.c
	${CC} -DLINUX -o nf-bootstrap ${SRCS}

upload: notforth.hex
	avrdude -v -b 57600 -D -c arduino -p atmega328p -P /dev/ttyUSB0 -U flash:w:notforth.hex

AVR_CCOPTS=-c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections -flto -fno-fat-lto-objects -mmcu=atmega328p -DF_CPU=16000000L -DAVR -D__AVR_ATmega168__ -I/usr/avr/sys-root/include
AVR_LDOPTS=-w -Os -g -flto -fuse-linker-plugin -Wl,--gc-sections -mmcu=atmega328p

notforth.hex: ${SRCS} ${HDRS} builtins/all.c
	avr-gcc ${AVR_CCOPTS} -mmcu=atmega328p -I/usr/avr/sys-root/include -c ${SRCS}
	avr-gcc ${AVR_LDOPTS} -o notforth.elf *.o
	avr-strip notforth.elf
	avr-objcopy -O ihex -R .eeprom notforth.elf notforth.hex

.SUFFIXES:
