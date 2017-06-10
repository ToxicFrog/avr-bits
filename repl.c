#include "config.h"

#include "repl.h"
#include "dictionary.h"
#include "tty.h"
#include "execute.h"
#include "lexer.h"

extern Word* compiling;
void repl() {
  while (1) {
    if (compiling) print("compile ");
    print("[");
    printint(STACKP);
    print("] ");
    lex_input();
    execute_bytecode((WordImpl*)pop());
  }
}
