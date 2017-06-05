#pragma once
#include <stdint.h>

typedef void (*WordImpl)(void);

// Definition of a !Forth Word
typedef struct Word {
  // Pointer to next entry in the dict.
  // For words packed into an array, or the last word in the dict, this is NULL.
  struct Word * next;
  // Pointer to the function to execute for this word. For precompiled words
  // this is going to be a C function of no args that implements it.
  // For RAM words this is going to be a Word*[] implementing it.
  WordImpl execute;
  // Pointer to the name of the word. For precompiled words this is a string
  // constant. For RAM words this is malloc'd somewhere, probably.
  const char * name;
  // Flags regulating how the word behaves. See WordFlags below.
  uint8_t flags;
} Word;

typedef enum WordFlags {
#ifdef LINUX
  // Host doesn't have harvard architecture, so nothing is ever "in flash"
  NEXT_IN_FLASH = 0,
  NAME_IN_FLASH = 0,
  SELF_IN_FLASH = 0,
#else
  // `next` pointer is to .progmem rather than to RAM
  NEXT_IN_FLASH = 1 << 0,
  // `name` pointer is to .progmem rather than to RAM; name is at most 32 bytes including null terminator.
  NAME_IN_FLASH = 1 << 1,
  // The word itself is stored in flash, and what we're looking at is a temporary copy in RAM.
  SELF_IN_FLASH = 1 << 2,
#endif
  // `execute` is a constant to be pushed now rather than a function pointer
  IS_CONSTANT   = 1 << 3,
  // `execute` is a pointer to an opcode array to be executed by call_word rather than a function pointer
  IS_WORDLIST   = 1 << 4,
  // Evaluating this word in compile mode executes it instead of compiling it
  IS_IMMEDIATE  = 1 << 5,
} WordFlags;

#define WORD_IN_ARRAY ((Word*)(~0))

extern Word* DICTIONARY;

Word* find_word(const char*);
Word* register_word(const char*, void (*)(void));
Word* next_word(Word*);
