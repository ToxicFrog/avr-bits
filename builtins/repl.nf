"repl.nf" c/file

( repl/ateof -- bool )
(Returns true if stdin is at end-of-file.)
:repl/ateof 'push(tty_peek() == EOF);' c/defn

( repl/lex-token -- * )
(Lex a token from stdin. The exact behaviour of this depends on context; in
command mode the token will be executed immediately whether or not it is
declared IMMEDIATE, in compilation mode it will be compiled and the generated
bytecode pushed onto the stack.)
:repl/lex-token 'lex_token();' c/defn

:repl/run {
  pop  ; there will be an error code on the stack (or 0 on first run)
  prompt
  ; lex-token will call prompt automatically at EOL.
  @repl/lex-token @repl/ateof until
} defn

:led-init { pins/D13 pin/out } defn
:led-on { pins/D13 1 pin/write } defn
:led-off { pins/D13 0 pin/write } defn

:main {
  led-init
  led-on
  0
  ; TODO: pcall crashes in hosted mode
  @repl/run @repl/run pcall
} defn

0 c/file
