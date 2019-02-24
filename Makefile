AVR=attiny85
CLK=16000000L
AVR_CCFLAGS=-c -Os -g -std=gnu11 -Wall \
	-isystem /usr/avr/sys-root/include \
  -mmcu=${AVR} -DF_CPU=${CLK}
AVR_LDFLAGS=-Os -w -flto -fuse-linker-plugin -Wl,--gc-sections -mmcu=${AVR}

all: main.hex

upload: mn-cli/micronucleus main.hex
	mn-cli/micronucleus --run main.hex

clean:
	rm -f *.o *.hex *.elf
	make -C mn-cli clean

size: main.elf
	avr-size -C main.elf | egrep --color=never '^(Program|Data)'

list: main.elf
	avr-objdump -h -S main.elf | less

mn-cli/micronucleus:
	make -C mn-cli

main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size -C main.elf | egrep --color=never '^(Program|Data)'

main.elf: main.c
	avr-gcc ${AVR_CCFLAGS} -c -o main.o $<
	avr-gcc ${AVR_LDFLAGS} -o main.elf main.o
# 	avr-strip main.elf

.SUFFIXES:
