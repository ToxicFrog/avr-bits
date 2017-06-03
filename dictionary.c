#include <stdlib.h>   // NULL, malloc
#include <stdint.h>
#include <string.h>   // strcmp, strlen

#include "dictionary.h"
#include "tty.h"
#include "stacks.h"

Word* DICTIONARY = NULL;

// Given a Word*, find the next word in the dictionary. Return NULL if this was the last one.
Word* next_word(Word* word) {
  if (word->flags & NEXT_IN_FLASH) {
    // FIXME
    return NULL;
  } else if (word->next == (Word*)(~0)) {
    return word-1;
  } else {
    return word->next;
  }
}

Word* find_word(const char* name) {
  Word* word = DICTIONARY;
  while (word) {
    // FIXME: check for NAME_IN_FLASH here
    if (strcmp(word->name, name) == 0) {
      return word;
    }
    word = next_word(word);
  }
  return NULL;
}

void execute_word(Word* word) {
  if (word->flags & IS_CONSTANT) {
    push((intptr_t)word->execute);
  } else if (word->flags & IS_WORDLIST) {
    // FIXME: execute wordlist
  } else {
    word->execute();
  }
}

void register_word(const char* name, void (*exec)(void)) {
  println(name);
  Word* word = malloc(sizeof(Word));
  word->next = DICTIONARY;
  word->execute = exec;
  word->name = name;
  DICTIONARY = word;
}

void register_constant(const char* name, const intptr_t value) {
  register_word(name, (void *)value);
  DICTIONARY->flags |= IS_CONSTANT;
}
