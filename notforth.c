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
#include "lexer.h"
#include "execute.h"

void run_file(const char* file) {
  stdin = fopen(file, "r");
  while (!feof(stdin)) {
    lex_input();
    execute_bytecode((WordImpl*)pop());
  }
  fclose(stdin);
}

int main(int argc, char ** argv) {
  load_core_words();

  if (argc > 1) {
    fclose(stdin);
    for (int i = 1; i < argc; ++i) {
      run_file(argv[i]);
    }
    return 0;
  }

  printf("argc: %d\n", argc);
  printf("Pointer size: %lu\n", sizeof(void*));
  printf("Word size: %lu\n", sizeof(Word));
  repl();
  return 0;
}

#endif  // HOST_NOTFORTH
