#include "common.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "compiler.h"
#include "execute.h"
#include "tty.h"
#include "error.h"

void lex_whitespace();
void lex_token();

void lex_linecomment();
void lex_blockcomment();
void lex_longstring();
void lex_shortstring();
void lex_number();
void lex_addressof();
void lex_word();

void lex_error() {
  // Unwind the compilation stack if we're in compile state.
  compile_abort();

  // Consume remaining input up to (and including) newline.
  for (int ch = tty_next(); ch != '\n' && ch != EOF; ch = tty_next());

  // Restore the old jump target and return from it.
  uncatch_error();
  qerror();
}

// Process a single token from the input.
// When called, leading whitespace has already been consumed, so tty_peek() returns
// the first character of the next token.
// Depending on what it finds, it either discards input (comments) or calls
// compile_foo(), which pushes appropriate bytecode onto the stack.
// In c/file mode, compile_foo() will also emit appropriate C code.
void lex_token() {
  if (catch_error()) return lex_error();

  // Drop input until the character we are *about* to read is either \n or non-whitespace.
  char ch;
  for (ch = tty_peek(); isspace(ch) && ch != '\n'; ch = tty_peek()) tty_next();
  switch (ch) {
    // Line comment.
    case ';':
      lex_linecomment();
      break;

    // Block comment.
    case '(':
      lex_blockcomment();
      break;

    // Addressof.
    case '@':
      lex_addressof();
      break;

    // String, possibly multiline.
    case '"':
    case '\'':
      lex_longstring();
      break;

    // Token string.
    case ':':
      lex_shortstring();
      break;

    // End of line.
    case '\n':
      tty_next();
      execute_word(find_word("prompt"));
      break;

    // End of input. Should never happen on AVR, might on Linux.
    case EOF:
      if (compiling)
        error("EOF while reading function body.");
      break;

    // Numbers and function calls.
    default:
      if (isdigit(ch)) {
        // No support for negatives yet.
        lex_number();
      } else {
        lex_word();
      }
      break;
  }

  // Yay! We processed a token without encountering any errors. Free up the
  // catchpoint we claimed at the start of the function.
  uncatch_error();
}

void lex_linecomment() {
  while (tty_peek() != '\n' && tty_peek() != EOF) tty_next();
}

void lex_blockcomment() {
  int depth = 0;
  char ch;
  do {
    ch = tty_next();
    if (ch == '(') ++depth;
    if (ch == ')') --depth;
  } while (depth);
}

// Read text into the stack until pred(tty_peek()) is true.
// Null terminate it and return a pointer to the start of the string.
// Note that any subsequent use of the stack invalidates the string.
char* readuntil(int (*pred)(int)) {
  char* buf = (char*)&STACK[STACKP];
  size_t len = 0;
  while (!pred(tty_peek())) {
    buf[len++] = tty_next();
    CHECK(STACKP + (len/sizeof(Cell)) < STACKSIZE, "Stack overflow reading string, number, or name.");
  }
  buf[len] = '\0';
  return buf;
}

// Equivalent to readuntil(), except that it then allocates a buffer for the
// string, copies the string into it, and returns a pointer to the buffer.
char* readuntila(int (*pred)(int)) {
  char* buf = readuntil(pred);
  char* str;
  CHECK_MALLOC(str, strlen(buf) + 1,
    "Failed to allocate memory for string literal.");
  strcpy(str, buf);
  return str;
}

char quote;
int isquote(int ch) { return ch == quote; }

void lex_longstring() {
  quote = tty_next();
  compile_string(readuntila(isquote));
  tty_next();
}

void lex_shortstring() {
  tty_next(); // discard leading :
  compile_string(readuntila(isspace));
}

void lex_number() {
  char* buf = readuntil(isspace);
  Cell num = strtol(buf, &buf, 0);
  if (*buf) {
    error("Invalid number: %s", buf);
    return;
  }
  compile_number(num);
}

void lex_addressof() {
  tty_next(); // Drop leading @
  char* name = readuntil(isspace);
  Word* word = find_word(name);
  if (!word) {
    error("Can't take address of undefined word '%s'", name);
    return;
  }
  compile_addressof(word);
}

void lex_word() {
  char* name = readuntil(isspace);
  Word* word = find_word(name);
  if (!word) {
    error("Can't call undefined word '%s'", name);
    return;
  }
  compile_word(word);
}
