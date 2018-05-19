"stack.nf" c/file

(Stack functions.)

( x dup -- x x )
(Duplicates the top element of the stack.)
:dup "push(peek());" c/defn

( x y exch -- y x )
(Swaps the top two stack elements.)
:exch "Cell x = pop(); Cell y = pop(); push(x); push(y);" c/defn

( x pop -- )
(Drops the top element of the stack.)
:pop "pop();" c/defn

0 c/file
