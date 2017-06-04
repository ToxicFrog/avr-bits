#include <stdlib.h>   // NULL, malloc
#include <stdint.h>
#include <string.h>   // strcmp, strlen

#include "dictionary.h"
#include "tty.h"
#include "stacks.h"

Word* DICTIONARY = NULL;

#ifndef HOST_NOTFORTH
static Word flashbuf;
#endif
// Given a Word*, find the next word in the dictionary. Return NULL if this was the last one.
Word* next_word(Word* word) {
#ifndef HOST_NOTFORTH
  if (word->flags & NEXT_IN_FLASH) {
    memcpy_P(&flashbuf, word->next, sizeof(Word));
    return &flashbuf;
    // TODO: check for NAME_IN_FLASH here too
  }
#endif
  return word->next;
}

Word* find_word(const char* name) {
  Word* word = DICTIONARY;
  while (word) {
    if (word->name && strcmp(word->name, name) == 0) {
      return word;
    }
    word = next_word(word);
  }
  return NULL;
}

Word* register_word(const char* name, void (*exec)(void)) {
  // print("register: "); println(name);
  Word* word = malloc(sizeof(Word));
  word->next = DICTIONARY;
  word->execute = exec;
  word->name = name;
  word->flags = 0x00;
  DICTIONARY = word;
  return word;
}
