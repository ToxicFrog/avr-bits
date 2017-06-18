#include "common.h"

#include "repl.h"
#include "dictionary.h"
#include "tty.h"
#include "execute.h"
#include "lexer.h"
#include "error.h"

extern Word* compiling;
void repl() {
  while(true) {
    printf_P(PSTR("[%u%s "), STACKP, compiling ? ">>" : "]");

    if (tty_peek() == EOF) break;
    if (!lex_input()) continue;
    WordImpl* code = (WordImpl*)pop();
    if (!setjmp(catchpoint)) {
        execute_bytecode(code);
    }
    free(code);
  };
}
