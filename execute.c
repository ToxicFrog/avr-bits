#include "common.h"

#include <stdlib.h>
#include <string.h>  // memcpy

#include "execute.h"
#include "dictionary.h"
#include "compiler.h"
#include "error.h"

// Core library that does the actual execution of words and provides functions
// for interacting with the stacks.

Cell STACK[STACKSIZE];
size_t STACKP = 0; // points to the empty slot just above the last stack slot

Cell peek() {
  CHECK(STACKP > 0, "stack underflow in peek()");
  return STACK[STACKP-1];
}

Cell pop() {
  CHECK(STACKP > 0, "stack underflow in pop()");
  return STACK[--STACKP];
}

void push(Cell val) {
  CHECK(STACKP < STACKSIZE, "stack overflow in push()");
  STACK[STACKP++] = val;
}

// Given a Word* pointer, execute it.
void execute_word(Word* word) {
  if (word->flags & IS_CONSTANT) {
    // Constants store the value in word->execute, so just push that.
    // Note that under normal circumstances, constants are resolved by the
    // compiler; this will only ever happen if the word has both WORD_CONSTANT
    // and WORD_IMMEDIATE set, and it will push the value of the constant,
    // WITHOUT THE LEADING OP_PUSHLITERAL OPCODE, onto the stack.
    push((intptr_t)word->execute);

  } else if (word->flags & IS_BYTECODE) {
    // If bytecode, word->execute points to an array of WordImpls terminated
    // with OP_EOF.
    execute_bytecode((WordImpl*)word->execute);
  } else {
    // Otherwise it's just a pointer to a C function of no args that does the thing.
    word->execute();
  }
}

void execute_bytecode(const WordImpl* bytecode) {
  // Each WordImpl is either:
  // OP_EOF: stop executing
  // OP_PUSHLITERAL <val>: push val onto the data stack
  // OP_CALLWORD <ptr>: treat ptr as a Word* and execute it
  // or a pointer to a C function to call.
  for (int IP = 0; bytecode[IP] != OP_EOF; ++IP) {
    if (bytecode[IP] == OP_PUSHLITERAL) {
      push((Cell)bytecode[++IP]);
    } else if (bytecode[IP] == OP_CALLWORD) {
      execute_bytecode((WordImpl*)bytecode[++IP]);
    } else {
      bytecode[IP]();
    }
  }
}
