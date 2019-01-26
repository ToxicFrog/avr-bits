(Words for defining words.)

"defn.nf" c/file

(name body defn -- )
(Bind the name to the function body. The body needs to be a code block, i.e.
{...}. To alias a function use :alias { orig } defn, not :alias @orig defn.)
:defn '
  Word* word = (Word*)pop();
  CHECK(!word->name, "tried to use defn to alias a function");
  word->name = (char*)pop();
  c_register_word(word);
' c/defn

(name body defmacro -- )
(As defn, but the function will be marked IMMEDIATE, meaning that it will be
  invoked as soon as it is lexed, rather than a call to it being compiled into
  bytecode. This allows it to arbitrarily alter the bytecode of the function
  being compiled; the function code compiled so far is on the stack.
  Note that REPL code is executed as soon as it's lexed, so calling macros
  from the REPL outside a {} block will probably not do what you want.)
:defmacro "
  ((Word*)peek())->flags |= IS_IMMEDIATE;
  word_defn_impl();
" c/defn

(name val defconst -- )
(As defn, but `val` need not be a function, and when `name` is invoked, it will
  be pushed onto the stack.)
(TODO: add a `defval` variant that's mutable.
  This will probably require a new dictionary flag, since right now when the
  compiler encounters a constant it emits the actual value into the bytecode
  along with a PUSHLITERAL instruction. This won't work with vars because the
  value may have changed by the time the function gets called...)
:defconst '
  WordImpl val = (WordImpl)pop();
  c_register_word(register_word((const char*)pop(), val, IS_CONSTANT));
' c/defn

(name size defvar -- )
(As defconst, but allocates `size` bytes of memory, and defines `name` as a
  constant holding the address of that memory.)
; TODO this doesn't actually work!
; when precompiling, it calls malloc() ON THE HOST, then embeds the resulting
; address into the generated C code!
;:defvar '
;  push((Cell)malloc((size_t)pop()));
;  word_defconst_impl();
;' c/defn

; TODO: words to clean up words? In particular, a way to free a word, and a way
; to unlink a word from the dictionary.

0 c/file
