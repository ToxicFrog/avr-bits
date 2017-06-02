// Core words for notforth.

// #include <stdio.h>
// #include <stdlib.h>

#include "dictionary.h"
#include "tty.h"
#include "stacks.h"

//// IO words ////

void word_printnum(Word* _) {
  printint(pop());
}

void word_stack(Word* _) {
  println("Stack size:");
  printint(SP);
  println("Stack contents:");
  for (int i = SP-1; i >= 0; --i) {
    printint(STACK[i]);
  }
  println("----");
}

//// Math words ////

void word_add(Word* _) {
  int x = pop(), y = pop();
  push(x+y);
}

void word_sub(Word* _) {
  int x = pop(), y = pop();
  push(x-y);
}

void word_mul(Word* _) {
  int x = pop(), y = pop();
  push(x*y);
}

void word_div(Word* _) {
  int x = pop(), y = pop();
  push(x/y);
}

void word_mod(Word* _) {
  int x = pop(), y = pop();
  push(x%y);
}

//// Memory words ////

void word_peek(Word* _) {
  int* ptr = (int*)((uint64_t)pop());
  push(*ptr);
}

void word_poke(Word* _) {
  int* ptr = (int*)((uint64_t)pop());
  int val = pop();
  *ptr = val;
}

void word_dup(Word* _) {
  push(peek());
}

//// Control words ////

void word_words(Word* word) {
  for (word = DICTIONARY; word; word = word->next) {
    println(word->name);
  }
}

#ifdef HOST_NOTFORTH
void word_bye(Word* _) {
  exit(0);
}
#endif

void load_core_words() {
  //register_word("bye", word_bye);
  println("Loading core words...");
  register_word(".", word_printnum);
  register_word("+", word_add);
  register_word("-", word_sub);
  register_word("*", word_mul);
  register_word("/", word_div);
  register_word("%", word_mod);
  register_word("?", word_peek);
  register_word("!", word_poke);
  register_word("stack", word_stack);
  register_word("words", word_words);
#ifdef HOST_NOTFORTH
  register_word("bye", word_bye);
#endif
}
