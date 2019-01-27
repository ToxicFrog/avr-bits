'morningtime.nf' c/file

:data 2 defvar

; Keep these flags on the stack for now because we don't have defvar.
:waiting? { data ? } defn
:waiting! { data exch ! } defn
:morning? { data 1 + ? } defn
:morning! { data 1 + exch ! } defn

:loop-body {
  { ; code for when button is pressed
    {
      ; clear the waiting flag until the button is released
      0 waiting!
      ; set red light and invert flag
      pins/D8 morning? pin/write
      1 morning? - morning!
    } waiting? if
  } { ; code for when button is released
    {
      ; set green light
      pins/D10 morning? pin/write
    } waiting? not if
    1 waiting!
  } pins/D11 pin/read ifelse
  1
} defn

:morningtime {
  ; initialize lights: red on, green off
  pins/D8 pin/out
  pins/D10 pin/out
  pins/D11 pin/in
  pins/D8 1 pin/write
  pins/D10 0 pin/write
  @loop-body loop
} defn

0 c/file
