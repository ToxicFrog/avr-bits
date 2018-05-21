"stack.nf" c/file

(Stack functions.)

( n nth -- val )
(Pops n, then counts down n spaces from the top of the stack and pushes a copy of that.)
:nth "size_t n = (size_t)pop(); push(STACK[STACKP-n-1]);" c/defn

( x dup -- x x )
(Duplicates the top element of the stack.)
:dup { 1 nth } defn

( <...> n dupn -- <...> <...> )
(Duplicates the top n stack elements in order.)
:dupn '
  const Cell n = pop();
  for (Cell i = n; i > 0; --i) {
    push(STACK[STACKP-n]);
  }
' c/defn

( x y exch -- y x )
(Swaps the top two stack elements.)
:exch "Cell x = pop(); Cell y = pop(); push(x); push(y);" c/defn

( x pop -- )
(Drops the top element of the stack.)
:pop "pop();" c/defn

(TODO: support nth, insert, delete, drop-n, dup-n, count)

0 c/file
