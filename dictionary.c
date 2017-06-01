#include <stdlib.h>   // NULL, malloc
#include <stdint.h>
#include <string.h>   // strcmp, strlen

#include "dictionary.h"
#include "tty.h"

Word* DICTIONARY = NULL;

Word* find_word(const char* name) {
  Word* word = DICTIONARY;
  while (word) {
    if (strcmp(word->name, name) == 0) {
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
  println(name);
  Word* word = malloc(sizeof(Word));
  word->next = DICTIONARY;
  word->execute = exec;
  word->name = name;
  DICTIONARY = word;
}
