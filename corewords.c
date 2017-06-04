// Core words for notforth.

// #include <stdio.h>
// #include <stdlib.h>

#include "dictionary.h"
#include "tty.h"
#include "stacks.h"

//// IO words ////

void word_printnum() {
  printint(pop()); println("");
}

void word_printstr() {
  println((const char*)pop());
}

void word_stack() {
  print("Stack size: "); printint(SP); println("");
  for (int i = SP-1; i >= 0; --i) {
    printint(STACK[i]); println("");
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

void word_const() {
  const char * name = (const char *)pop();
  register_constant(name, pop());
}

#ifdef HOST_NOTFORTH
void word_bye() {
  exit(0);
}
#endif

// the { that begins a function definition
// switches the core into compile mode, if it's not already there, and pushes
// a new, empty wordlist function onto the definition stack (can this be the same as the data stack?)
void word_beginfn() {
  defn_begin();
}

// the } that ends a function definition
// switches the core out of compile mode, if this is the outermost function we're defining
// allocates a Word for the function and pushes it onto the stack
// The Word is added to the dictionary so it can be freed later, but has no name
// To give it a name, use defn once it's on the stack.
void word_endfn() {
  push((Cell)defn_end());
}

void word_defn() {
  Word* fn = (Word*)pop();
  char* name = (char*)pop();
  fn->name = name;
}

void load_core_words() {
  //register_word("bye", word_bye);
  println("Loading core words...");
  register_word(".", word_printnum);
  register_word("s.", word_printstr);
  register_word(".s", word_stack);
  register_word("dup", word_dup);
  register_word("+", word_add);
  register_word("const", word_const);
  register_word("-", word_sub);
  register_word("*", word_mul);
  register_word("/", word_div);
  register_word("%", word_mod);
  register_word("?", word_peek);
  register_word("!", word_poke);
  register_word("words", word_words);
  register_word("{", word_beginfn);
  register_word("}", word_endfn)->flags |= IS_IMMEDIATE;
  register_word("defn", word_defn);
#ifdef HOST_NOTFORTH
  register_word("bye", word_bye);
#endif
}
