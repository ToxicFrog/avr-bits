(Words for defining words.)

"00-defn.nf" c/file

(name body defn -- )
(Bind the name to the function body. The body can either be an inline function
  literal, e.g. :inc { 1 + } defn, or the value of an existing function, e.g.
  :println @. defn)
:defn "
  Word* word = (Word*)pop();
  word->name = (char*)pop();
  c_register_word(word);
" c/defn

(name body defmacro -- )
(As defn, but the function will be marked IMMEDIATE, meaning that it will be
  invoked as soon as it is lexed, rather than a call to it being compiled into
  bytecode. This allows it to arbitrarily alter the bytecode of the function
  being compiled. Note that stuff typed into the REPL also counts as a function!
  The function-start and function-end words `{` and `}` are implemented as
  IMMEDIATE words.)
:defmacro "
  ((Word*)peek())->flags |= IS_IMMEDIATE;
  word_defn_impl();
" c/defn

(name val defconst -- )
(As defn, but `val` need not be a function, and when `name` is invoked, it will
  be pushed onto the stack.)
:defconst '
  WordImpl val = (WordImpl)pop();
  register_word((const char*)pop(), val, IS_CONSTANT);
' c/defn

(name size defvar -- )
(As defconst, but allocates `size` bytes of memory, and defines `name` as a
  constant holding the address of that memory.)
# TODO :defvar { alloc defconst } defn

# TODO: words to clean up words? In particular, a way to free a word, and a way
# to unlink a word from the dictionary.

:exec 'execute_word((Word*)pop());' c/defn

0 c/file
