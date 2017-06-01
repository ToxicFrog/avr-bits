#pragma once
#include <stdint.h>

// Definition of a !Forth Word
typedef struct Word {
  // Pointer to next entry in the dict; NULL at end of dict.
  struct Word * next;
  // Pointer to the function to execute for this word. For precompiled words
  // this is going to be a C function of no args that implements it.
  // For words compiled to RAM this is going to be a pointer to a function
  // that takes the Word* as its sole argument and interprets it.
  void (*execute)(struct Word *);
  // Pointer to the name of the word. For precompiled words this is a string
  // constant. For RAM words this is malloc'd somewhere, probably.
  const char * name;
  // unused right now
  // uint8_t flags;
  // uint8_t data[];
} Word;

extern Word* DICTIONARY;

Word* find_word(const char*);
void execute_word(Word*);
void register_word(const char*, void (*)(Word*));
