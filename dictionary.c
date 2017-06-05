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
  static Word wordbuf;
  if (word->flags & NEXT_IN_FLASH) {
    memcpy_P(&wordbuf, word->next, sizeof(Word));
    return &wordbuf;
  }
  return word->next;
}

int word_name_eq(const Word* word, const char* name) {
  if (!word->name) return false;
  if (word->flags & NAME_IN_FLASH) {
    return strcmp_P(name, word->name) == 0;
  }
  return strcmp(name, word->name) == 0;
}

Word* find_word(const char* name) {
  Word* word = DICTIONARY;
  while (word) {
    if (word_name_eq(word, name)) {
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
