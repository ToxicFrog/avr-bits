"99-led.nf" c/file

:DDRB 0x24 defconst
:PORTB 0x25 defconst

(LED is pin D13, which is bit 5 of IO register B)
:led-init { DDRB dup ? 5 bit-on ! } defn
:led-on { PORTB dup ? 5 bit-on ! } defn
:led-off { PORTB dup ? 5 bit-off ! } defn

0 c/file
