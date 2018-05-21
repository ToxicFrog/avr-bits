(Functions for manipulating AVR digital and analog GPIO pins.)

'avr-pins.nf' c/file

; register offsets
:pin/PIN 0 defconst
:pin/DDR 1 defconst
:pin/PORT 2 defconst

( pin reg-offs pin/split -- reg-addr bit-index )
(Given a pin ID and a register offset, push the register memory address and the
bit index within that register of the pin.)
:pin/split {
  1 nth ; copy pin to top of stack
  0xFF00 & 8 >> + ; extract register address, add offset
  exch 0x00FF & ; extract pin bit
} defn

( pin pin/in -- )
(Configure pin for input.)
:pin/in {
  ; get register and bit
  pin/DDR pin/split
  ; read current register value
  1 nth ?
  ; clear bit and write
  exch bit-off !
} defn

( pin pin/read -- val )
(Read a value from pin. Pin must be configured for input.)
:pin/read {
  pin/PIN pin/split
  exch ?
  exch bit-on?
} defn

( pin pin/out -- )
(Configure pin for output.)
:pin/out {
  ; get register and bit
  pin/DDR pin/split
  ; read current register value
  1 nth ?
  ; set bit and write
  exch bit-on !
} defn

( pin val pin/write -- )
(Write a value to a pin. Pin must be configured for output.)
:pin/write {
  exch pin/PORT pin/split
    ;; val reg bit
  1 nth ?
    ;; val reg bit currval
  exch 3 nth
    ;; val reg currval bit val
  bit-set
    ;; val reg newval
  ! pop
} defn

:pin/dummy { } defn

0 c/file
