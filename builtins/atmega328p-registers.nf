"atmega328p-registers.nf" c/file

; Ok, this is super gross.
; <avr/io.h> defines a bunch of names for the various IO registers, e.g.
; PINB, DDRB, PORTB
; The problem is that these aren't constexprs, so we can't compile them into
; PROGMEM word entries -- we have to register them at runtime.
; This eats 7 bytes of RAM (plus malloc overhead) per register.
; So instead we do this and just hard-code the values for now. :/

0
:PINB 0x23 :DDRB 0x24 :PORTB 0x25
:PINC 0x26 :DDRC 0x27 :PORTC 0x28
:PIND 0x29 :DDRD 0x2a :PORTD 0x2b

@defconst @dup while

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
