#include "config.h"

#include <stdlib.h>
#include <string.h>  // memcpy

#include "stacks.h"
#include "dictionary.h"

// Core library that does the actual execution of words and provides functions
// for interacting with the stacks.

Cell STACK[STACKSIZE];
size_t STACKP = 0; // points to the empty slot just above the last stack slot

// Stack of Word definitions that we're compiling. When we finish a word, it
// gets popped from this stack and added to DICTIONARY, albeit without a name.
// It's up to the user to call defn to give it one, if they want.
Word* compiling = NULL;

#define OP_EOF ((WordImpl)0x0000)
#define OP_PUSHLITERAL ((WordImpl)0x0001)
#define OP_CALLWORD ((WordImpl)0x0002)

Cell peek() {
  return STACK[STACKP-1];
}

Cell pop() {
  return STACK[--STACKP];
}

void push(Cell val) {
  if (compiling) {
    STACK[STACKP++] = (Cell)OP_PUSHLITERAL;
    compiling->flags += 2;  // one for PUSHLITERAL and one for the actual value
  }
  STACK[STACKP++] = val;
}

Word flashbuf;

void execute_word(Word* word) {
  if (compiling && !(word->flags & IS_IMMEDIATE)) {
    if (word->flags & IS_WORDLIST) {
      STACK[STACKP++] = (Cell)OP_CALLWORD;
      compiling->flags++;
    }
    STACK[STACKP++] = (Cell)word->execute;
    compiling->flags++;
    return;
  }

  // Immediate execution, or not in compile mode.
  if (word->flags & IS_CONSTANT) {
    // Constants store the value in word->execute, so just push that.
    push((intptr_t)word->execute);

  } else if (word->flags & IS_WORDLIST) {
    // Wordlists make word->execute a pointer to an array of WordImpls.
    // Each one is either:
    // OP_EOF: stop executing
    // OP_PUSHLITERAL <val>: push val onto the data stack
    // OP_CALLWORD <ptr>: treat ptr as a Word* and execute it
    // or a pointer to a WordImpl that should be called directly.
    WordImpl* opcodes = (WordImpl*)word->execute;
    for (int IP = 0; opcodes[IP] != OP_EOF; ++IP) {
      if (opcodes[IP] == OP_PUSHLITERAL) {
        push((Cell)opcodes[++IP]);
      } else if (opcodes[IP] == OP_CALLWORD) {
        execute_word((Word*)opcodes[++IP]);
      } else {
        opcodes[IP]();
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
  memcpy(body, &STACK[STACKP-len], len * sizeof(Cell));
  word->execute = (WordImpl)body;

  // Drop the wordlist from the stack.
  STACKP -= len;

  // Set flags.
  word->flags = IS_WORDLIST;

  // Link the word into the dictionary.
  word->next = DICTIONARY;
  DICTIONARY = word;

  return word;
}

