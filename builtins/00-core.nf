(IO words)

"00-core.nf" c/file

(print int)
:. 'printf_P(PSTR("%lu\n"), (unsigned long)pop());' c/defn

(print char*)
:s. 'puts((const char*)pop());' c/defn

(dump stack contents)
:.s '
  printf_P(PSTR("Stack size: %lu\n"), (unsigned long)STACKP);
  for (int i = STACKP-1; i >= 0; --i) {
    printf_P(PSTR("  [%2u] %lu\n"), i, (unsigned long)(STACK[i]));
  }
' c/defn

:bye '
#ifdef LINUX
  exit(0);
#else
  void (*reset)(void) = NULL;
  reset();
#endif
' c/defn

(memory and stack functions)

( addr ? -- *addr )
:? "push(*(Cell*)pop());" c/defn

( addr val ! -- )
:! "Cell val = pop(); *(Cell*)pop() = val;" c/defn

( x dup -- x x )
:dup "push(peek());" c/defn

( x y exch -- y x )
:exch "Cell x = pop(); Cell y = pop(); push(x); push(y);" c/defn

( ifcode cond if -- )
:if '
  Cell cond = pop();
  Word* ifword = (Word*)pop();
  if (cond) execute_word(ifword);
' c/defn

( ifcode elsecode cond ifelse -- )
:ifelse '
  Cell cond = pop();
  Word* elseword = (Word*)pop();
  Word* ifword = (Word*)pop();
  if (cond) {
    execute_word(ifword);
  } else {
    execute_word(elseword);
  }
' c/defn

0 c/file
