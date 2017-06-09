(Builtin words implemented in C.)

"00-core.nf" c/file


(math words)

:+ "push(pop() + pop());" c/defn
:* "push(pop() * pop());" c/defn
:- "Cell y = pop(); push(pop()-y); " c/defn
:/ "Cell y = pop(); push(pop()/y); " c/defn
:% "Cell y = pop(); push(pop()%y); " c/defn


(IO words)

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


(words for defining words)

:defn "Word* word = (Word*)pop(); word->name = (char*)pop();" c/defn

:const '
  WordImpl val = (WordImpl)pop();
  register_word((const char*)pop(), val)->flags |= IS_CONSTANT | IS_IMMEDIATE;
' c/defn

:immediate "((Word*)peek())->flags |= IS_IMMEDIATE;" c/defn


0 c/file
bye
