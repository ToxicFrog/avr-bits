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

// Top level command to process input and turn it into bytecode.
// Repeatedly eats whitespace, then reads and compiles a token.
// If the whitespace includes \n or hits EOF and there are no unfinished functions
// on the compilation stack, finalizes and pushes the bytecode for what it's
// processed so far and returns true.
// If there are and it hits \n it keeps reading.
// If it hits EOF and the compilation stack is nonempty, it returns false.
// On success, leaves on top of the stack a pointer to a bytecode buffer suitable for
// execution with execute_bytecode(). Caller takes ownership and should free it.
int lex_input() {
  size_t old_sp = STACKP;
  if (setjmp(catchpoint)) {
    compile_abort();
    STACKP = old_sp;
    // consume remaining input or the user is gonna have a bad time
    while (tty_peek() != '\n' && tty_peek() != EOF) tty_next();
    if (tty_peek() == '\n') tty_next();
    return false;
  }

  while(true) {
    lex_whitespace();
    if (tty_peek() == '\n') {
      // End of line. If not inside a function, finish this lexer cycle.
      tty_next();
      if (!compiling) break;
    } else if (tty_peek() == EOF) {
      // End of input. Like \n but die if we can't finish.
      if (!compiling) break;
      error("EOF inside function definition.");
      return false;
    } else {
      lex_token();
    }
  }

  // We finished reading a bunch of input! malloc() up a buffer for the generated
  // bytecode and push a pointer to it.
  compile_eof();
  size_t len = (STACKP - old_sp) * sizeof(WordImpl);
  WordImpl* bytecode;
  CHECK_MALLOC(bytecode, len,
    "Failed to allocate bytecode buffer for user input.");
  memcpy(bytecode, &STACK[old_sp], len);
  STACKP = old_sp;
  push((Cell)bytecode);
  return true;
}

// Consume input up to newline, EOF, or non-whitespace.
void lex_whitespace() {
  // Drop input until the character we are *about* to read is either \n or non-whitespace.
  for (char ch = tty_peek(); isspace(ch) && ch != '\n'; ch = tty_peek()) tty_next();
}

// Process a single token from the input.
// When called, leading whitespace has already been consumed, so tty_peek() returns
// the first character of the next token.
// Depending on what it finds, it either discards input (comments) or calls
// compile_foo(), which pushes appropriate bytecode onto the stack.
// In c/file mode, compile_foo() will also emit appropriate C code.
void lex_token() {
  char ch = tty_peek();
  switch (ch) {
    // Line comment.
    case '#': return lex_linecomment();
    // Block comment.
    case '(': return lex_blockcomment();
    // Addressof.
    case '@': return lex_addressof();
    // String, possibly multiline.
    case '"':
    case '\'':
      return lex_longstring();
    // Token string.
    case ':': return lex_shortstring();
    default: break;
  }

  // If we get here it's either a number or a word.
  // No support for negatives as yet, it makes distinguishing a negative number
  // and the word `-` tricky.
  if (isdigit(ch)) {
    return lex_number();
  }

  return lex_word();
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
    print("At input: "); println(buf);
    error("Invalid number.");
    return;
  }
  compile_number(num);
}

void lex_addressof() {
  tty_next(); // Drop leading @
  char* name = readuntil(isspace);
  Word* word = find_word(name);
  if (!word) {
    print("At input: @"); println(name);
    error("Attempt to take address of undefined word.");
    return;
  }
  compile_addressof(word);
}

void lex_word() {
  char* name = readuntil(isspace);
  Word* word = find_word(name);
  if (!word) {
    print("At input: "); println(name);
    error("Attempt to call undefined word");
    return;
  }
  compile_word(word);
}
