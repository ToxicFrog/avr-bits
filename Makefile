AVR=attiny85
CLK=16500000L
AVR_CCFLAGS=-c -Os -g -std=gnu11 -Wall \
	-isystem /usr/avr/sys-root/include \
  -mmcu=${AVR} \
  -DF_CPU=${CLK}
AVR_LDFLAGS=-Os -w -flto -fuse-linker-plugin -Wl,--gc-sections -mmcu=${AVR}

all: main.size morningtime.size

%.upload: %.hex mn-cli/micronucleus
	mn-cli/micronucleus --run $<

clean:
	rm -f *.o *.hex *.elf usbdrv/*.o
	make -C mn-cli clean

mn-cli/micronucleus:
	make -C mn-cli

%.size: %.elf
	avr-size -C $< | egrep --color=never '^(Program|Data)'

%.list: %.elf
	avr-objdump -h -S $< | less

%.hex: %.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-size -C $@ | egrep --color=never '^(Program|Data)'

# HACK HACK HACK
# It should infer this from the stem, but this doesn't seem to be possible or something?!
main.elf: PROGNAME = main
morningtime.elf: PROGNAME = morningtime

%.elf: %.o usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o
	avr-gcc ${AVR_LDFLAGS} -o $@ $^

# Same deal here, it should depend on a usbconfig file depending on which binary we're building,
# but it doesn't, oh well.
%.o: %.c usbdrv/usbconfig_*.h
	avr-gcc ${AVR_CCFLAGS} -DUSB_CONFIGURATION_FILE='"usbconfig_${PROGNAME}.h"' -o $@ $<

%.o: %.S usbdrv/usbconfig_*.h
	avr-gcc ${AVR_CCFLAGS} -DUSB_CONFIGURATION_FILE='"usbconfig_${PROGNAME}.h"' -o $@ $<

.SUFFIXES:
