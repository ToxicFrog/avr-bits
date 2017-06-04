#include <stdlib.h>
#include <string.h>  // memcpy

#include "stacks.h"
#include "dictionary.h"

// Core library that does the actual execution of words and provides functions
// for interacting with the stacks.

Cell STACK[32];
Cell SP = 0; // points to the empty slot just above the last stack slot

// Stack of Word definitions that we're compiling. When we finish a word, it
// gets popped from this stack and added to DICTIONARY, albeit without a name.
// It's up to the user to call defn to give it one, if they want.
Word* compiling = NULL;

#define WORD_EOF ((Word*)0x0000)
#define WORD_PUSHLITERAL ((Word*)0x0001)

Cell peek() {
  return STACK[SP-1];
}

Cell pop() {
  return STACK[--SP];
}

void push(Cell val) {
  if (compiling) {
    STACK[SP++] = (Cell)WORD_PUSHLITERAL;
    compiling->flags += 2;  // one for PUSHLITERAL and one for the actual value
  }
  STACK[SP++] = val;
}

void execute_word(Word* word) {
  if (compiling && !(word->flags & IS_IMMEDIATE)) {
    STACK[SP++] = (Cell)word;
    compiling->flags++;
    return;
  }

  if (word->flags & IS_CONSTANT) {
    push((intptr_t)word->execute);
  } else if (word->flags & IS_WORDLIST) {
    Word** opcodes = (Word**)word->execute;
    for (int IP = 0; opcodes[IP] != WORD_EOF; ++IP) {
      if (opcodes[IP] == WORD_PUSHLITERAL) {
        push((Cell)opcodes[++IP]);
      } else {
        execute_word(opcodes[IP]);
      }
    }
  } else {
    word->execute();
  }
}

// Create a new, blank Word definition and put it on the compilation stack.
//
void defn_begin() {
  Word* word = malloc(sizeof(Word));
  word->next = compiling;
  word->execute = NULL;
  word->name = NULL;
  word->flags = 0;
  compiling = word;
}

Word* defn_end() {
  if (!compiling) return NULL; //die("} with no corresponding {");
  // Pop it from the compilation stack.
  Word* word = compiling;
  compiling = word->next;
  // Copy the wordlist off the stack.
  // The top word->flags stack cells contain instructions.
  // This relies on Cell and Word* having the same size, but since Cell is intptr_t that should be the case?
  size_t len = (size_t)(word->flags);
  Word* body = calloc(len + 1, sizeof(Word*));
  memcpy(body, &STACK[SP-len], len * sizeof(Cell));
  word->execute = (WordImpl)body;
  // Drop the wordlist from the stack.
  SP -= len;
  // Set flags.
  word->flags = IS_WORDLIST;
  // Link the word into the dictionary.
  word->next = DICTIONARY;
  DICTIONARY = word;

  return word;
}

