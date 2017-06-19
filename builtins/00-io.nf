(Terminal IO functions.)

"00-io.nf" c/file

(print int)
(super weirdness: this gets registered as :. rather than as :u.)
:u. 'printf_P(PSTR("%lu\n"), (unsigned long)pop());' c/defn

(print signed int)
:i. 'printf_P(PSTR("%ld\n"), (signed long)pop());' c/defn

(print hex int)
:x. 'printf_P(PSTR("%p\n"), (void*)pop());' c/defn
:. { x. } defn

(print char*)
:s. 'puts((const char*)pop());' c/defn

(dump stack contents)
:.s '
  printf_P(PSTR("Stack size: %lu\n"), (unsigned long)STACKP);
  for (int i = STACKP-1; i >= 0; --i) {
    printf_P(PSTR("  [%2u] %lu\n"), i, (unsigned long)(STACK[i]));
  }
' c/defn

0 c/file
