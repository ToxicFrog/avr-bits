(output words)

(display an int)
:C . (int . -- ) {
  int n = pop();
  printf("%d\n", n);
}

(math words)

:C + (int int -- int) {
  int x = pop(), y = pop();
  push(x+y);
}

:C - (int int -- int) {
  int x = pop(), y = pop();
  push(x-y);
}

:C * (int int -- int){
  int x = pop(), y = pop();
  push(x*y);
}

:C / (int int -- int) {
  int x = pop(), y = pop();
  push(x/y);
}

:C % (int int -- int) {
  int x = pop(), y = pop();
  push(x%y);
}

(memory words)

(peek)
:C ? (addr -- int) {
  int* ptr = (int*)((intptr_t)pop());
  push(*ptr);
}

(poke)
:C ! (int addr --) {
  int* ptr = (int*)((uint64_t)pop());
  int val = pop();
  *ptr = val;
}

:C dup (int -- int int) {
  push(peek());
}

//// Control words ////

: words (--) C{
  for (word = DICTIONARY; word; word = word->next) {
    printf("%s ", (char*)word->data);
  }
  printf("\n");
}

: bye (--) C{
  exit(0);
}
