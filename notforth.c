// Notforth main file for HOST. See notforth.ino for TARGET.

#ifdef HOST_NOTFORTH

#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define false 0x00
#define true (~false)

#include "dictionary.h"
#include "repl.h"

//#define debuglog printf
#define debuglog(...) ;

// #include "core.c"

int main() {
  printf("Pointer size: %lu\n", sizeof(void*));
  printf("Word size: %lu\n", sizeof(Word));
  printf("Offsets: 0x%lx 0x%lx 0x%lx 0x%lx\n",
    offsetof(Word, next), offsetof(Word, execute), offsetof(Word, flags),
    offsetof(Word, data));
  // register_word("bye", word_bye);
  // register_word(".", word_printnum);
  // register_word("+", word_add);
  // register_word("-", word_sub);
  // register_word("*", word_mul);
  // register_word("/", word_div);
  // register_word("%", word_mod);
  // register_word("?", word_peek);
  // register_word("!", word_poke);
  // register_word("words", word_words);
  while (true) {
    printf("] ");
    process_line();
  }
  return 0;
}

#endif  // HOST_NOTFORTH
