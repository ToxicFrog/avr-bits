#include "common.h"

#include "repl.h"
#include "dictionary.h"
#include "tty.h"
#include "execute.h"
#include "lexer.h"
#include "error.h"

void repl() {
  if (catch_error()) {
    printf_P(PSTR("Error in execution.\n"));
  }
  while(tty_peek() != EOF) {
    lex_token();
  }
  uncatch_error();
}
