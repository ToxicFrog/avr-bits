"atmega328p-registers.nf" c/file

; Ok, this is super gross.
; <avr/io.h> defines a bunch of names for the various IO registers, e.g.
; PINB, DDRB, PORTB
; The problem is that these aren't constexprs, so we can't compile them into
; PROGMEM word entries -- we have to register them at runtime.
; This eats 7 bytes of RAM (plus malloc overhead) per register.
; So instead we do this and just hard-code the values for now. :/

0
:PINB :DDRB :PORTB
:PINC :DDRC :PORTC
:PIND :DDRD :PORTD
0x2b ; address of last register

; TODO: replace with for loop, once for loops implemented
; something like:
; @defconst 0x2b 0x23 -1 for
; except we don't have negatives, so 0 1 - I guess
{
  ; TODO: a way to disable C emission for individual functions so that helpers
  ; like these don't end up cluttering up the final symbol table.
  ; define register
  2 dupn defconst
  ; decrement address
  1 -
  ; pop register name
  exch pop
} { 1 nth } while
pop pop



(
'#ifdef AVR
#include <avr/io.h>
' c/append

:registers/init '

' c/defn

0
:PINB :DDRB :PORTB
:PINC :DDRC :PORTC
:PIND :DDRD :PORTD
@c/import @dup while

'#endif  // AVR
' c/append
)

0 c/file
