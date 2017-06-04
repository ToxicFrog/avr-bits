#pragma once
#include <stdint.h>

typedef void (*WordImpl)(void);

// Definition of a !Forth Word
typedef struct Word {
  // Pointer to next entry in the dict; NULL at end of dict.
  // If ~0x0, this Word is part of an array; the next word immediately precedes
  // it in memory.
  struct Word * next;
  // Pointer to the function to execute for this word. For precompiled words
  // this is going to be a C function of no args that implements it.
  // For RAM words this is going to be a Word*[] implementing it.
  WordImpl execute;
  // Pointer to the name of the word. For precompiled words this is a string
  // constant. For RAM words this is malloc'd somewhere, probably.
  const char * name;
  // Flags regulating how the word behaves. See WordFlags below.
  uint16_t flags;
} Word;

typedef enum WordFlags {
  // `next` pointer is to .progmem rather than to RAM
  NEXT_IN_FLASH = 1 << 0,
  // `name` pointer is to .progmem rather than to RAM; name is at most 32 bytes including null terminator.
  NAME_IN_FLASH = 1 << 1,
  // `execute` is a constant to be pushed now rather than a function pointer
  IS_CONSTANT   = 1 << 2,
  // `execute` is a pointer to a Word*[] to be repeatedly fed to call_word() rather than a function pointer
  IS_WORDLIST   = 1 << 3,
  // Evaluating this word in compile mode executes it instead of appending it
  IS_IMMEDIATE  = 1 << 4,
} WordFlags;

#define WORD_IN_ARRAY ((Word*)(~0))

extern Word* DICTIONARY;

Word* find_word(const char*);
Word* register_word(const char*, void (*)(void));
Word* next_word(Word*);
