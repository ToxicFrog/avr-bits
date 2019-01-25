; SYNTAX TEST "notforth.sublime-syntax"

  (commment)
; ^ punctuation.definition.comment
;  ^ comment.block
;          ^ punctuation.definition.comment

; single-line comment
; <- punctuation.definition.comment
; ^ comment.line

0xff
; <- constant.numeric.hex

255 ; end of line comment
; <- constant.numeric
;   ^ punctuation.definition.comment
;     ^ comment.line

:square { dup * } defn
; ^ entity.name
;       ^ punctuation.section.block.begin
;         ^ meta.block
;               ^ punctuation.section.block.end

@square list
; ^ entity.name

{ 5 + }
; ^ constant.numeric
;^^^^^ meta.block

"here is a string" .
; <- punctuation.definition.string.begin
; ^ string.quoted.double
;                ^ punctuation.definition.string.end

'here is a
  multiline
string' .
; ^ string.quoted.single
;     ^ punctuation.definition.string.end

"here is a string with ( a paren"
; <- punctuation.definition.string.begin
;                      ^ string.quoted.double
;                               ^ punctuation.definition.string.end

{
  (block comment)
; ^^^^^^^^^^^^^^^^ punctuation.definition.comment
  5 5 + ; line comment
;       ^^^^^^^^^^^^^^^ punctuation.definition.comment
}
