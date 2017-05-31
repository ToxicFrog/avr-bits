#include "dictionary.h"

// Core library that does the actual execution of words and provides functions
// for interacting with the stacks.

Word* DICTIONARY = NULL;

Word* find_word(const char* name) {
  Word* word = DICTIONARY;
  while (word) {
    if (strcmp((char*)(&word->data), name) == 0) {
      return word;
    }
    word = word->next;
  }
  printf("Word not found in dictionary: %s\n", name);
  return NULL;
}

void execute_word(Word* word) {
  word->execute(word);
}

void register_word(const char* name, void (*exec)(Word*)) {
  printf("Register: %p %s\n", exec, name);
  Word* word = malloc(sizeof(word)+strlen(name)+1);
  word->next = DICTIONARY;
  word->execute = exec;
  word->flags = 0x0;
  strcpy((char*)(&word->data), name);
  DICTIONARY = word;
}

