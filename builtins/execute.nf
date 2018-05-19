(IO words)

"execute.nf" c/file

( bye -- )
(On linux, exits the interpreter. On AVR, resets the board.)
:bye '
#ifdef LINUX
  exit(0);
#else
  void (*reset)(void) = NULL;
  reset();
#endif
' c/defn

( fn err pcall -- * )
(Call `fn`. If `fn` throws an error, pushes the error code on the stack and
calls `err`.)
:pcall '
  Word* err = (Word*)pop();
  Word* fn = (Word*)pop();
  if (catch_error()) {
    push(1);
    execute_word(err);
  } else {
    execute_word(fn);
  }
  uncatch_error();
' c/defn

( fn call -- * )
(Call the function on top of the stack. If it's not a function, interprets it
as one and then bad things happen.)
:call 'execute_word((Word*)pop());' c/defn

0 c/file
