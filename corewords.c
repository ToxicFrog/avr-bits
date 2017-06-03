// Core words for notforth.

// #include <stdio.h>
// #include <stdlib.h>

#include "dictionary.h"
#include "tty.h"
#include "stacks.h"

//// IO words ////

void word_printnum() {
  printint(pop());
}

void word_stack() {
  print("Stack size: "); printint(SP);
  for (int i = SP-1; i >= 0; --i) {
    printint(STACK[i]);
  }
  println("----");
}

//// Math words ////

void word_add() {
  int x = pop(), y = pop();
  push(x+y);
}

void word_sub() {
  int x = pop(), y = pop();
  push(x-y);
}

void word_mul() {
  int x = pop(), y = pop();
  push(x*y);
}

void word_div() {
  int x = pop(), y = pop();
  push(x/y);
}

void word_mod() {
  int x = pop(), y = pop();
  push(x%y);
}

//// Memory words ////

void word_peek() {
  int* ptr = (int*)((uint64_t)pop());
  push(*ptr);
}

void word_poke() {
  int* ptr = (int*)((uint64_t)pop());
  int val = pop();
  *ptr = val;
}

void word_dup() {
  push(peek());
}

//// Control words ////

void word_words() {
  for (Word* word = DICTIONARY; word; word = word->next) {
    println(word->name);
  }
}

#ifdef HOST_NOTFORTH
void word_bye() {
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
