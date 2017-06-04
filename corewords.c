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
  print("Stack size: "); printint(STACKP); println("");
  for (int i = STACKP-1; i >= 0; --i) {
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
  for (Word* word = DICTIONARY; word; word = next_word(word)) {
    printint((intptr_t)word); print(":"); printint((intptr_t)word->execute);
    print(" "); print(word->name); print(" ["); printint(word->flags); println("]");
  }
}

void word_const() {
  const WordImpl value = (WordImpl)pop();
  register_word((const char*)pop(), value)->flags |= IS_CONSTANT;
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

Word CORE_WORDS[] = {
  { NULL, word_printnum, ".", 0 },
  { WORD_IN_ARRAY, word_printstr, "s.", 0 },
  { WORD_IN_ARRAY, word_stack, ".s", 0 },

  { WORD_IN_ARRAY, word_beginfn, "{", 0 },
  { WORD_IN_ARRAY, word_endfn, "}", IS_IMMEDIATE },
  { WORD_IN_ARRAY, word_defn, "defn", 0 },
  { WORD_IN_ARRAY, word_const, "const", 0 },

  { WORD_IN_ARRAY, word_dup, "dup", 0 },

  { WORD_IN_ARRAY, word_add, "+", 0 },
  { WORD_IN_ARRAY, word_sub, "-", 0 },
  { WORD_IN_ARRAY, word_mul, "*", 0 },
  { WORD_IN_ARRAY, word_div, "/", 0 },
  { WORD_IN_ARRAY, word_mod, "%", 0 },

  { WORD_IN_ARRAY, word_peek, "?", 0 },
  { WORD_IN_ARRAY, word_poke, "!", 0 },

#ifdef HOST_NOTFORTH
  { WORD_IN_ARRAY, word_bye, "bye", 0 },
#endif
};

void load_core_words() {
  DICTIONARY = CORE_WORDS + (sizeof(CORE_WORDS)/sizeof(Word)) - 1;
  register_word("words", word_words);//->flags |= NEXT_IN_FLASH;
}
