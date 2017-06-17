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

void printstr_P(const char * str) {
  char ch[2] = { 0, 0 };
  while ((ch[0] = pgm_read_byte(str++))) {
    print(ch);
  }
}

void printname(const Word* word) {
  if (!word->name) {
    print("(anonymous)");
    return;
  }
  if (word->flags & NAME_IN_FLASH) {
    printstr_P(word->name);
  } else {
    print(word->name);
  }
}

void word_words() {
  for (Word* word = DICTIONARY; word; word = next_word(word)) {
    printint((intptr_t)word); print(":"); printint((intptr_t)word->execute);
    print(" ");
    printname(word);
    print(" ["); printint(word->flags); println("]");
  }
}

void impl_to_wordname(WordImpl impl) {
  for (Word* word = DICTIONARY; word; word = next_word(word)) {
    if (word->execute == impl) {
      print(" :"); printname(word);
      return;
    }
  }
}

void word_list() {
  Word* word = (Word*)pop();
  print("# word "); printname(word); println("");
  if (word->flags & IS_CONSTANT) {
    print("constant "); printint((intptr_t)word->execute); println("");
  } else if (word->flags & IS_BYTECODE) {
    WordImpl* op = (WordImpl*)word->execute;
    while (*op != NULL) {
      if (*op == (WordImpl)0x0001) {
        print("pushliteral "); printint((intptr_t)*(++op)); println("");
      } else if (*op == (WordImpl)0x0002) {
        print("call word@"); printint((intptr_t)*(++op)); print(" "); printname((Word*)*op); println("");
      } else {
        print("call C@"); printint((intptr_t)*op); impl_to_wordname(*op); println("");
      }
      ++op;
    }
  } else {
    print("call C@"); printint((intptr_t)word->execute); println("");
  }
  println("# end");
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
  register_word("words", word_words)->flags |= NEXT_IN_FLASH;
}
