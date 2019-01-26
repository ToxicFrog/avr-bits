'demo.nf' c/file

; simple demo that turns the red LED (on D8) on and the green one (D10) off
; then swaps them when you hold down the button (D11)
:demo {
  ; D8 is the red LED, D10 is the green one, D11 is the button
  pins/D8 dup pin/out 1 pin/write
  pins/D10 dup pin/out 0 pin/write
  pins/D11 pin/in
  { ; button read loop
    pins/D11 pin/read dup
    pins/D10 exch pin/write
    1 exch - pins/D8 exch pin/write
    1
  } loop
} defn

0 c/file
