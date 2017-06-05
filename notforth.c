// Notforth main file for HOST. See notforth.ino for TARGET.

#include "config.h"

#ifdef LINUX

#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "dictionary.h"
#include "repl.h"
#include "corewords.h"

int main() {
  printf("Pointer size: %lu\n", sizeof(void*));
  printf("Word size: %lu\n", sizeof(Word));
  load_core_words();
  repl();
  return 0;
}

#endif  // HOST_NOTFORTH
