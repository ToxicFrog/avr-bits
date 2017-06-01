#include <stdlib.h>   // NULL, malloc
#include <stdint.h>
#include <string.h>   // strcmp, strlen

#include "dictionary.h"

Word* DICTIONARY = NULL;

Word* find_word(const char* name) {
  Word* word = DICTIONARY;
  while (word) {
    if (strcmp((char*)(&word->data), name) == 0) {
      return word;
    }
    word = word->next;
  }
  return NULL;
}

void execute_word(Word* word) {
  word->execute(word);
}

void register_word(const char* name, void (*exec)(Word*)) {
  Word* word = malloc(sizeof(word)+strlen(name)+1);
  word->next = DICTIONARY;
  word->execute = exec;
  word->flags = 0x0;
  strcpy((char*)(&word->data), name);
  DICTIONARY = word;
}

