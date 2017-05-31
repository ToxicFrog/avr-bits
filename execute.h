#pragma once

#include <stdint.h>

// Definition of a !Forth Word
typedef struct Word {
  // Pointer to next entry in the dict; NULL at end of dict.
  struct Word * next;
  // Pointer to the interpreter for this word. For C words this points to the
  // C function itself and the argument is unused.
  // For !Forth words it points to the threaded interpreter.
  void (*execute)(struct Word *);
  // Whether the word has compile-time semantics, etc.
  uint8_t flags;
  // This is the variable-length part and is kind of grody.
  // The first (flags & WORD_NAMELEN_MASK) bytes are the name of the word.
  // For C words, this is all that's needed.
  // For !Forth words, this is then followed by:
  // - a byte containing the number of words in the body NW
  // - zero padding to make it machineword-aligned
  // - NW pointers to words which make up the function body.
  uint8_t data[];
} Word;

extern Word* DICTIONARY;
extern int STACK[];
extern int SP;

int pop();
int peek();
void push(int);

Word* find_word(const char*);
void execute_word(Word*);
void register_word(const char*, void (*)(Word*));
