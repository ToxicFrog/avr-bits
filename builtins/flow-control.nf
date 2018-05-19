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

( body expr while -- )
(Repeatedly evaluates expr, then, if it pushed a true value onto the stack, body.
If the body is expected to leave the value to be checked on the stack, make sure
there's something there before invoking while and then use @dup as the expr.
To iterate over a list of values, assuming you can use false as a sentinel, you
can do something like:
  0 x y z { ... } @dup while pop)
(TODO: investigate whether I want this to be <body> <initial-value> while and then
expect body to leave something on the stack.)
:while '
  Word* expr = (Word*)pop();
  Word* body = (Word*)pop();
  for (execute_word(expr); pop(); execute_word(expr)) {
    execute_word(body);
  }
' c/defn

( body expr until -- )
(Evaluates body, then repeatedly evaluates expr and then body until expr returns true.)
:until '
  Word* expr = (Word*)pop();
  Word* body = (Word*)pop();
  do {
    execute_word(body);
    execute_word(expr);
  } while (!pop());
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
