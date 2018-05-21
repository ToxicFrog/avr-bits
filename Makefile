## Compiler configuration
# N.b. all settings assume your board is an atmega328p based one.
# If it's an atmega88 or something you'll have to change some things.
CCFLAGS=-std=gnu11 -Wall -Werror -g -O0 -fno-optimize-sibling-calls -Iinclude
AVR_CCFLAGS=-c -Os -w -std=gnu11 -mmcu=atmega328p \
	-ffunction-sections -fdata-sections -flto -fno-fat-lto-objects \
	-DF_CPU=16000000L -DAVR -D__AVR_ATmega168__ \
	-isystem /usr/avr/sys-root/include -Iinclude
AVR_LDFLAGS=-Os -w -flto -fuse-linker-plugin -Wl,--gc-sections -mmcu=atmega328p

## File sets
SRCS=$(wildcard src/*.c)
HDRS=$(wildcard include/*.h)
OBJS=$(SRCS:src/%.c=build/%.o)
WORDS=$(wildcard builtins/*.nf)

## Top-level targets
all: notforth

test: notforth
	./notforth TEST

debug: notforth
	gdb -ex run notforth

run: notforth
	./notforth

## Write it to a connected Arduino on ttyUSB0
upload: build/notforth.hex
	avrdude -v -b 57600 -D -c arduino -p atmega328p -P /dev/ttyUSB0 -U flash:w:build/notforth.hex

clean:
	> builtins/all.c
	rm -f notforth nf-bootstrap builtins/*.nf.impl build/*

## Hosted version
# Builds the whole thing in one shot without leaving any intermediate files around.
notforth: ${SRCS} ${HDRS} builtins/all.c
	gcc ${CCFLAGS} -DLINUX -o notforth ${SRCS}

## Builtin libraries.
# Generating these is complicated enough that we outsource it to a shell script.
builtins/all.c: nf-bootstrap builtins.sh builtins/LIST ${WORDS}
	bash builtins.sh

## Minimal bootstrap binary.
# TODO: disable builtins while building.
nf-bootstrap: ${SRCS} ${HDRS}
	touch builtins/all.c
	gcc ${CCFLAGS} -DLINUX -o nf-bootstrap ${SRCS}

## ROM image for the AVR.
build/notforth.hex: ${OBJS}
	avr-gcc ${AVR_LDFLAGS} -o build/notforth.elf build/*.o
	avr-strip build/notforth.elf
	avr-objcopy -O ihex -R .eeprom build/notforth.elf build/notforth.hex

build/corewords.o: src/corewords.c builtins/all.c
	avr-gcc ${AVR_CCFLAGS} -c -o $@ $<

build/%.o: src/%.c
	avr-gcc ${AVR_CCFLAGS} -c -o $@ $<

.SUFFIXES:
