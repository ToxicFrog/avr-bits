// Notforth main file for HOST. See notforth.ino for TARGET.

#ifdef HOST_NOTFORTH

#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define false 0x00
#define true (~false)

#include "dictionary.h"
#include "repl.h"
#include "corewords.h"

//#define debuglog printf
#define debuglog(...) ;

// #include "core.c"

int main() {
  printf("Pointer size: %lu\n", sizeof(void*));
  printf("Word size: %lu\n", sizeof(Word));
  load_core_words();
  repl();
  return 0;
}

#endif  // HOST_NOTFORTH
