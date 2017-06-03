// C implementation of the data stack and functions to manipulate it.

#pragma once

#include <stdint.h>

typedef intptr_t Cell;
extern Cell STACK[];
extern Cell SP;

Cell pop();
Cell peek();
void push(Cell);
