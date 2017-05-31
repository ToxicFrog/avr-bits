// Core words for notforth.

#include <stdio.h>
#include <stdlib.h>

#include "execute.h"

//// IO words ////

void word_printnum(Word* _) {
  int n = pop();
  printf("%d\n", n);
}

//// Math words ////

void word_add(Word* _) {
  int x = pop(), y = pop();
  push(x+y);
}

void word_sub(Word* _) {
  int x = pop(), y = pop();
  push(x-y);
}

void word_mul(Word* _) {
  int x = pop(), y = pop();
  push(x*y);
}

void word_div(Word* _) {
  int x = pop(), y = pop();
  push(x/y);
}

void word_mod(Word* _) {
  int x = pop(), y = pop();
  push(x%y);
}

//// Memory words ////

void word_peek(Word* _) {
  int* ptr = (int*)((uint64_t)pop());
  push(*ptr);
}

void word_poke(Word* _) {
  int* ptr = (int*)((uint64_t)pop());
  int val = pop();
  *ptr = val;
}

void word_dup(Word* _) {
  push(peek());
}

//// Control words ////

void word_words(Word* word) {
  for (word = DICTIONARY; word; word = word->next) {
    printf("%s ", (char*)word->data);
  }
  printf("\n");
}

void word_bye(Word* _) {
  exit(0);
}
