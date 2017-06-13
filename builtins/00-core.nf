(IO words)

"00-core.nf" c/file

(print int)
:. 'printint(pop()); println("");' c/defn

(print char*)
:s. 'println((const char*)pop());' c/defn

(dump stack contents)
:.s '
  print("Stack size: "); printint(STACKP); println("");
  for (int i = STACKP-1; i >= 0; --i) {
    printint(STACK[i]); println(""); }
  println("----");
' c/defn


(memory and stack functions)

( addr ? -- *addr )
:? "push(*(Cell*)pop());" c/defn

( addr val ! -- )
:! "Cell val = pop(); *(Cell*)pop() = val;" c/defn

:dup "push(peek());" c/defn


0 c/file
bye
