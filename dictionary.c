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
    println("next_word: NEXT_IN_FLASH not implemented");
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

Word* register_constant(const char* name, const intptr_t value) {
  Word* word = register_word(name, (void *)value);
  word->flags |= IS_CONSTANT;
  return word;
}
