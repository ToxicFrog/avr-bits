#include "config.h"

#include <stdlib.h>
#include <string.h>  // memcpy

#include "execute.h"
#include "dictionary.h"
#include "compiler.h"

// Core library that does the actual execution of words and provides functions
// for interacting with the stacks.

Cell STACK[STACKSIZE];
size_t STACKP = 0; // points to the empty slot just above the last stack slot

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

// Given a Word* pointer, execute it.
void execute_word(Word* word) {
  // Are we in compilation mode? If so just push the bytecode for this word onto
  // the stack, unless it's marked IMMEDIATE.
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

  } else if (word->flags & IS_BYTECODE) {
    // If bytecode, word->execute points to an array of WordImpls terminated
    // with OP_EOF.
    execute_wordlist((WordImpl*)word->execute);
  } else {
    // Otherwise it's just a pointer to a C function of no args that does the thing.
    word->execute();
  }
}

void execute_wordlist(const WordImpl* bytecode) {
  // Each WordImpl is either:
  // OP_EOF: stop executing
  // OP_PUSHLITERAL <val>: push val onto the data stack
  // OP_CALLWORD <ptr>: treat ptr as a Word* and execute it
  // or a pointer to a C function to call.
  for (int IP = 0; bytecode[IP] != OP_EOF; ++IP) {
    if (bytecode[IP] == OP_PUSHLITERAL) {
      push((Cell)bytecode[++IP]);
    } else if (bytecode[IP] == OP_CALLWORD) {
      execute_word((Word*)bytecode[++IP]);
    } else {
      bytecode[IP]();
    }
  }
}
