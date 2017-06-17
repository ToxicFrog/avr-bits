// Notforth main file for HOST. See notforth.ino for TARGET.

#include "common.h"

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
#include "error.h"

void run_file(const char* file) {
  stdin = fopen(file, "r");
  while (!feof(stdin)) {
    if (!lex_input()) break;
    if (setjmp(catchpoint)) break;
    WordImpl* bytecode = (WordImpl*)pop();
    execute_bytecode(bytecode);
    free(bytecode);
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

  repl();
  return 0;
}

#endif  // HOST_NOTFORTH
