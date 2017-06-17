(math words)

"00-math.nf" c/file

:+ "push(pop() + pop());" c/defn
:* "push(pop() * pop());" c/defn
:- "Cell y = pop(); push(pop()-y); " c/defn
:/ "Cell y = pop(); push(pop()/y); " c/defn
:% "Cell y = pop(); push(pop()%y); " c/defn

(bitwise operators)

:& "push(pop() & pop());" c/defn
:| "push(pop() | pop());" c/defn
:^ "push(pop() ^ pop());" c/defn

( x amount << -- x' )
:<< "Cell n = pop(); push(pop() << n);" c/defn

( x amount >> -- x' )
:>> "Cell n = pop(); push(pop() >> n);" c/defn

( x ~ -- x' )
:~ "push(~pop());" c/defn

( these depend on C functions; define later
( x bit bit-on? -- truefalse )
:bit-on? { >> 1 bit-and } defn

( x bit bit-on -- x' )
:bit-on { 1 exch << | } defn

( x bit bit-off -- x' )
:bit-off { 1 exch << ~ & } defn
)

0 c/file
