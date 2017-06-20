// Core words for notforth.

#include "common.h"

#include "dictionary.h"
#include "tty.h"
#include "execute.h"
#include "compiler.h"

#define ENABLE_BUILTINS
#include "builtins/all.c"
#undef ENABLE_BUILTINS

//// Control words ////

void printname(const Word* word) {
  if (!word->name) {
    fputs_P(PSTR("(anonymous)"), stdout);
  } else if (word->flags & NAME_IN_FLASH) {
    fputs_P(word->name, stdout);
  } else {
    fputs(word->name, stdout);
  }
}

void printnamel(const Word* word) {
  printname(word);
  putchar('\n');
}

void word_words() {
  for (Word* word = DICTIONARY; word; word = next_word(word)) {
    printf_P(PSTR("%4lx:%4lx [%2x] "), (unsigned long)word, (unsigned long)word->execute, word->flags);
    printnamel(word);
  }
}

void impl_to_wordname(WordImpl impl) {
  for (Word* word = DICTIONARY; word; word = next_word(word)) {
    if (word->execute == impl) {
      fputs_P(PSTR("  # "), stdout); printnamel(word);
      return;
    }
  }
  putchar('\n');
}

void word_list() {
  Word* word = (Word*)pop();
  fputs_P(PSTR("## "), stdout); printname(word);

  if (word->flags & IS_CONSTANT) {
    printf_P(PSTR(" :: constant %lu\n"), (unsigned long int)word->execute);

  } else if (word->flags & IS_BYTECODE) {
    printf_P(PSTR(" :: bytecode @ %p\n"), word->execute);

    for (WordImpl* ip = (WordImpl*)word->execute; *ip != OP_EOF; ++ip) {
      if (*ip == OP_PUSHLITERAL) {
        printf_P(PSTR("push %lu\n"), (unsigned long int)*(++ip));
      } else if (*ip == OP_CALLWORD) {
        printf_P(PSTR("call bytecode @ %p"), *(++ip));
        impl_to_wordname(*ip);
      } else {
        printf_P(PSTR("call native @ %p"), *ip);
        impl_to_wordname(*ip);
      }
    }

  } else {
    printf_P(PSTR(" :: native code @ %p\n"), word->execute);
  }
}

const PROGMEM Word CORE_WORDS[] = {
  { (Word*)1, word_beginfn, "{", NEXT_IN_FLASH | SELF_IN_FLASH | IS_IMMEDIATE },
  { (Word*)1, word_endfn, "}", NEXT_IN_FLASH | SELF_IN_FLASH | IS_IMMEDIATE},

  #define ENABLE_BUILTINS
  #include "builtins/all.h"
  #undef ENABLE_BUILTINS

#ifdef LINUX
  { (Word*)1, word_cfile, "c/file", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)1, word_cdefn, "c/defn", NEXT_IN_FLASH | SELF_IN_FLASH },
#endif

  { NULL, word_list, "list", SELF_IN_FLASH },
};

void load_core_words() {
  DICTIONARY = (Word*)CORE_WORDS;  // cast to remove the const
  // We place `words` outside flash both because we need at least one dictionary
  // entry in RAM to contain the first NEXT_IN_FLASH, and because it's useful to
  // have available ~always for troubleshooting issues with flash dictionary traversal.
  register_word("words", word_words, NEXT_IN_FLASH);
}
