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


; pin definitions
; top byte is address of PIN io register (add 1 to get DDR, 2 to get PORT)
; bottom byte is bit within IO register
0
:pins/D0 0x2900
:pins/D1 0x2901
:pins/D2 0x2902
:pins/D3 0x2903
:pins/D4 0x2904
:pins/D5 0x2905
:pins/D6 0x2906
:pins/D7 0x2907

:pins/D8 0x2300
:pins/D9 0x2301
:pins/D10 0x2302
:pins/D11 0x2303
:pins/D12 0x2304
:pins/D13 0x2305

(not yet supported
:pins/A0 0x2600
:pins/A1 0x2601
:pins/A2 0x2602
:pins/A3 0x2603
:pins/A4 0x2604
)

@defconst @dup while


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
