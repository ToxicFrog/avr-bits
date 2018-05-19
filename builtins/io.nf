(Terminal IO functions.)

"io.nf" c/file

( n u. -- )
(Print n as an unsigned long int.)
:u. 'printf_P(PSTR("%lu\n"), (unsigned long)pop());' c/defn

( n i. -- )
(Print n as a signed long int.)
:i. 'printf_P(PSTR("%ld\n"), (signed long)pop());' c/defn

( n x. -- )
(Print n as a hexadecimal word.)
:x. 'printf_P(PSTR("%p\n"), (void*)pop());' c/defn
:. { x. } defn

( str s. -- )
(Print str as a null-terminated string.)
:s. 'puts((const char*)pop());' c/defn

( .s -- )
(Dump the contents of the stack as a series of unsigned longs.)
:.s '
  printf_P(PSTR("Stack size: %lu\n"), (unsigned long)STACKP);
  for (int i = STACKP-1; i >= 0; --i) {
    printf_P(PSTR("  [%2u] %lu\n"), i, (unsigned long)(STACK[i]));
  }
' c/defn

(TODO: a variant of .s that takes a print function to apply to each stack
element.)

0 c/file
