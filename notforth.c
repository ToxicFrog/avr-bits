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
#include "corewords.h"
#include "lexer.h"
#include "execute.h"
#include "error.h"

void silent_prompt() {}

void run_file(const char* file) {
  stdin = fopen(file, "r");
  if (catch_error()) {
    printf("Error executing file '%s'\n", file);
    fclose(stdin);
    return;
  }

  while (!feof(stdin)) {
    lex_token();
  }
  fclose(stdin);
  uncatch_error();
}

int main(int argc, char ** argv) {
  load_core_words();

  if (argc > 1) {
    register_word("prompt", silent_prompt, 0);
    fclose(stdin);
    for (int i = 1; i < argc; ++i) {
      run_file(argv[i]);
    }
    return 0;
  }

  execute_word(find_word("repl"));
  return 0;
}

#endif
