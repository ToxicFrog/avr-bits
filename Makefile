AVR=attiny85
CLK=16500000L
AVR_CCFLAGS=-c -Os -g -std=gnu11 -Wall \
	-isystem /usr/avr/sys-root/include \
  -mmcu=${AVR} -DF_CPU=${CLK}
AVR_LDFLAGS=-Os -w -flto -fuse-linker-plugin -Wl,--gc-sections -mmcu=${AVR}

all: main.hex

upload: mn-cli/micronucleus main.hex
	mn-cli/micronucleus --run main.hex

clean:
	rm -f *.o *.hex *.elf usbdrv/*.o
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

main.elf: main.o usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o
	avr-gcc ${AVR_LDFLAGS} -o $@ $^

%.o: %.c usbdrv/usbconfig.h
	avr-gcc ${AVR_CCFLAGS} -o $@ $<

%.o: %.S usbdrv/usbconfig.h
	avr-gcc ${AVR_CCFLAGS} -o $@ $<

.SUFFIXES:
