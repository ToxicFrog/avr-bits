"memory.nf" c/file

( addr ? -- *addr )
(Peek a byte.)
:? "push(*(uint8_t*)pop());" c/defn

( addr ?? -- *addr )
(Peek a uint16.)
:?? "push(*(uint16_t*)pop());" c/defn

( addr val ! -- )
(Poke a byte.)
:! "uint8_t val = (uint8_t)pop(); *(uint8_t*)pop() = val;" c/defn

( addr val !! -- )
(Poke a uint16.)
:!! "uint16_t val = (uint16_t)pop(); *(uint16_t*)pop() = val;" c/defn

(TODO: memory management
  malloc/free
  defvar to define a named pointer to an unintialized memory region
  e.g. :foo 6 defvar  ; define foo as pointer to byte[6]
  which expands to:
  :foo 6 malloc defconst
  perhaps we don't need defvar?
  some sort of initialized array creation; this syntax looks nicest:
  [ 1 2 3 ]
  but then we need to find the [, which may not be reliable unless we store it
  off stack, e.g. in the compilation stack
  so probably instead we want something like:
  1 2 3 3 []
  or even
  1 2 3 3 [byte]
  1 2 3 3 [word]
  i.e. the signature is  ... size [byte] -- pointer
  this turns into dup malloc and then some loop wackiness to copy the
  contents of the stack into the allocated memory.
  it might be easier to support strings with hex escapes for some purposes?)

0 c/file
