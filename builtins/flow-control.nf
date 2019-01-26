"flow-control.nf" c/file

(Built in flow control constructs.)

( ifcode cond if -- )
(Executes ifcode iff cond is true.)
:if '
  Cell cond = pop();
  Word* ifword = (Word*)pop();
  if (cond) execute_word(ifword);
' c/defn

( ifcode elsecode cond ifelse -- )
(Executes ifcode iff cond is true; otherwise executes elsecode.)
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

( body loop -- )
(Pops body and executes it, then pops the value on top of the stack. If it's 0,
returns. Otherwise, repeats the process until it's 0.)
:loop '
  Word* body = (Word*)pop();
  do {
    execute_word(body);
  } while (pop());
' c/defn

( body start end for -- )
(Repeatedly executes body, pushing values between start [inclusive] and end [exclusive] before
each execution; begins with start, and counts up/down by 1 each iteration.)
:for '
  Cell end = pop();
  Cell start = pop();
  Word* body = (Word*)pop();
  int delta = start < end ? 1 : -1;
  for (; start != end; start += delta) {
    push(start);
    execute_word(body);
  }
' c/defn

0 c/file
