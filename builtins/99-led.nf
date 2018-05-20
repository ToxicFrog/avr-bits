"99-led.nf" c/file

; IO pin memory map
; DDRx (Data Direction Register) control whether a given pin is read or write
; PORTx is pin output; it can be written to set the output or read to return the current output setting
; PINx is pin input and can only be read.

; D is pins 0-7
; B is pins 8-13
; C is analog pins 0-5

; per https://www.arduino.cc/en/Hacking/PinMapping168
; and the atmega168/328 datasheet
; D0-4 are PD0-4
; D5-7 are PD5-7
; D8-13 are PB0-5
; A0-5 are PC0-PC5
; D3,5,6,9,10,11 are PWM-enabled

:PINB 0x23 defconst
:DDRB 0x24 defconst
:PORTB 0x25 defconst
:PINC 0x26 defconst
:DDRC 0x27 defconst
:PORTC 0x28 defconst
:PIND 0x29 defconst
:DDRD 0x2A defconst
:PORTD 0x2B defconst

:defpin { 2 [byte] defconst } defn

:pin/D0 PIND 0 defpin
:pin/D1 PIND 0 defpin

(LED is pin D13, which is bit 5 of IO register B)
:led-init { DDRB dup ? 5 bit-on ! } defn
:led-on { PORTB dup ? 5 bit-on ! } defn
:led-off { PORTB dup ? 5 bit-off ! } defn

; HOF to negate a boolean function. WIP -- needs new syntax
(f negate -- f')
;:negate { { `1 not } } defn

:testloop {
  0 16 32 { dup u. . } @dup while
} defn

0 c/file
