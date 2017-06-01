#include "stacks.h"

// Core library that does the actual execution of words and provides functions
// for interacting with the stacks.

int STACK[32];
int SP = 0;

int peek() {
  return STACK[SP];
}

int pop() {
  return STACK[SP--];
}

void push(int val) {
  STACK[++SP] = val;
}
