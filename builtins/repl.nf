"repl.nf" c/file

( repl/ateof -- bool )
(Returns true if stdin is at end-of-file.)
:repl/not-eof? 'push(tty_peek() != EOF);' c/defn

( repl/lex-token -- * )
(Lex a token from stdin. The exact behaviour of this depends on context; in
command mode the token will be executed immediately whether or not it is
declared IMMEDIATE, in compilation mode it will be compiled and the generated
bytecode pushed onto the stack.)
:repl/lex-token 'lex_token();' c/defn

:repl {
  pop  ; there will be an error code on the stack (or 0 on first run)
  prompt
  ; lex-token will call prompt automatically at EOL.
  { repl/lex-token repl/not-eof? } loop
} defn

0 c/file
