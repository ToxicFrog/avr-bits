#include "common.h"

#include "repl.h"
#include "dictionary.h"
#include "tty.h"
#include "execute.h"
#include "lexer.h"
#include "error.h"

void repl() {
  catch_error();
  printf_P(PSTR("Entering REPL."));
  execute_word(find_word("prompt"));
  while(tty_peek() != EOF) {
    lex_token();
  }
  uncatch_error();
}
