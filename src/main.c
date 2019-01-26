// Notforth main file for hosted execution.
// See avr-main.c for AVR and notforth.ino for Arduino.

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

int run_file(const char* file) {
  stdin = fopen(file, "r");
  if (catch_error()) {
    printf("Error executing file '%s'\n", file);
    fclose(stdin);
    return 0;
  }

  while (!feof(stdin)) {
    lex_token();
  }
  fclose(stdin);
  uncatch_error();
  return 1;
}

int main(int argc, char ** argv) {
  load_core_words();

  if (argc > 1) {
    register_word("prompt", silent_prompt, 0);
    fclose(stdin);
    for (int i = 1; i < argc; ++i) {
      if (!run_file(argv[i]))
        return 1;
    }
    return 0;
  }

  execute_word(find_word("main"));
  return 0;
}

#endif  // LINUX
