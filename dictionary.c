#include "config.h"

#include <stdlib.h>   // NULL, malloc
#include <stdint.h>
#include <string.h>   // strcmp, strlen

#include "dictionary.h"
#include "tty.h"
#include "stacks.h"

Word* DICTIONARY = NULL;

// Given a Word*, find the next word in the dictionary. Return NULL if this was the last one.
Word* next_word(Word* word) {
#ifdef ARDUINO
  static Word flashbuf;
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
  Word* word = malloc(sizeof(Word));
  word->next = DICTIONARY;
  word->execute = exec;
  word->name = name;
  word->flags = 0x00;
  DICTIONARY = word;
  return word;
}
