// Core words for notforth.

#include "config.h"

#ifdef ARDUINO
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

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

#ifdef LINUX
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

const PROGMEM Word CORE_WORDS[] = {
  { NULL, word_printnum, ".", SELF_IN_FLASH },
  { (Word*)(CORE_WORDS+0), word_printstr, "s.", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)(CORE_WORDS+1), word_stack, ".s", NEXT_IN_FLASH | SELF_IN_FLASH },

  { (Word*)(CORE_WORDS+2), word_beginfn, "{", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)(CORE_WORDS+3), word_endfn, "}", NEXT_IN_FLASH | SELF_IN_FLASH | IS_IMMEDIATE},
  { (Word*)(CORE_WORDS+4), word_defn, "defn", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)(CORE_WORDS+5), word_const, "const", NEXT_IN_FLASH | SELF_IN_FLASH },

  { (Word*)(CORE_WORDS+6), word_dup, "dup", NEXT_IN_FLASH | SELF_IN_FLASH },

  { (Word*)(CORE_WORDS+7), word_add, "+", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)(CORE_WORDS+8), word_sub, "-", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)(CORE_WORDS+9), word_mul, "*", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)(CORE_WORDS+10), word_div, "/", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)(CORE_WORDS+11), word_mod, "%", NEXT_IN_FLASH | SELF_IN_FLASH },

  { (Word*)(CORE_WORDS+12), word_peek, "?", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)(CORE_WORDS+13), word_poke, "!", NEXT_IN_FLASH | SELF_IN_FLASH },

#ifdef LINUX
  #define LAST_DICT_IDX 15
  { (Word*)(CORE_WORDS+14), word_bye, "bye", NEXT_IN_FLASH | SELF_IN_FLASH },
#else
  #define LAST_DICT_IDX 14
#endif
};

#define LAST_DICT CORE_WORDS

void load_core_words() {
  DICTIONARY = (Word*)&LAST_DICT[LAST_DICT_IDX];  // cast to remove the const
  register_word("words", word_words)->flags |= NEXT_IN_FLASH;
}
