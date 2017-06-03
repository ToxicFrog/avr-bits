#include "stacks.h"

// Core library that does the actual execution of words and provides functions
// for interacting with the stacks.

Cell STACK[32];
Cell SP = 0; // points to the empty slot just above the last stack slot

Cell peek() {
  return STACK[SP-1];
}

Cell pop() {
  return STACK[--SP];
}

void push(Cell val) {
  STACK[SP++] = val;
}
