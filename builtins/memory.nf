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

0 c/file
