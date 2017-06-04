// C implementation of the data stack and functions to manipulate it.

#pragma once

#include <stdint.h>

#include "dictionary.h"

typedef intptr_t Cell;
extern Cell STACK[];
extern size_t STACKP;

Cell pop();
Cell peek();
void push(Cell);

void execute_word(Word*);

void defn_begin();
Word* defn_end();
