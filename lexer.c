#include "config.h"

#include <ctype.h>

#include "lexer.h"
#include "compiler.h"
#include "execute.h"

// FIXME use setjmp/longjmp here
#include <stdlib.h>
#include "tty.h"
#define error(x) println(x); abort()

void lex_whitespace(char (*peek)(void), char (*next)(void));
void lex_token(char (*peek)(void), char (*next)(void));

void lex_linecomment(char (*peek)(void), char (*next)(void));
void lex_blockcomment(char (*next)(void));
void lex_longstring(char (*peek)(void), char (*next)(void));
void lex_shortstring(char (*peek)(void), char (*next)(void));
void lex_number(char (*peek)(void), char (*next)(void));
void lex_addressof(char (*peek)(void), char (*next)(void));
void lex_word(char (*peek)(void), char (*next)(void));

// Top level command to process input and turn it into bytecode.
// Repeatedly eats whitespace, then reads and compiles a token.
// If the whitespace includes \n or hits EOF and there are no unfinished functions on the compilation
// stack, finalizes and pushes the bytecode for what it's pushed so far and returns.
// If there are and it hits \n it keeps reading.
// If it hits EOF and the compilation stack is nonempty, it throws.
// On success, leaves on top of the stack a pointer to a bytecode buffer suitable for
// execution with execute_bytecode(). Caller takes ownership and should free it.
// Contract of arguments:
// peek() returns the next character that would be read, without affecting the input stream. Returns \0 at EOF.
// next() returns the next character and advances the input stream. Throws at EOF.
// The latter is what causes things like eof-inside-string to do the right thing.
void lex_input(char (*peek)(void), char (*next)(void)) {
  size_t old_sp = STACKP;
  // FIXME: set up error handling here

  while(true) {
    lex_whitespace(peek, next);
    if (peek() == '\n') {
      // End of line. If not inside a function, finish this lexer cycle.
      next();
      if (!compiling) break;
    } else if (peek() == '\0') {
      // End of input. Like \n but die if we can't finish.
      if (!compiling) break;
      STACKP = old_sp;
      push((Cell)NULL);
      error("EOF inside function definition.");
      return;
    } else {
      lex_token(peek, next);
    }
  }

  // We finished reading a bunch of input! malloc() up a buffer for the generated
  // bytecode and push a pointer to it.
  compile_eof();
  size_t len = (STACKP - old_sp) * sizeof(WordImpl);
  WordImpl* bytecode = malloc(len);
  memcpy(bytecode, &STACK[old_sp], len);
  STACKP = old_sp;
  push((Cell)bytecode);
}

// Consume input up to newline, EOF, or non-whitespace.
void lex_whitespace(char (*peek)(void), char (*next)(void)) {
  // Drop input until the character we are *about* to read is either \n or non-whitespace.
  for (char ch = peek(); isspace(ch) && ch != '\n'; ch = peek()) next();
}

// Process a single token from the input.
// When called, leading whitespace has already been consumed, so peek() returns
// the first character of the next token.
// Depending on what it finds, it either discards input (comments) or calls
// compile_foo(), which pushes appropriate bytecode onto the stack.
// In c/file mode, compile_foo() will also emit appropriate C code.
void lex_token(char (*peek)(void), char (*next)(void)) {
  char ch = peek();
  switch (ch) {
    // Line comment.
    case '#': return lex_linecomment(peek, next);
    // Block comment.
    case '(': return lex_blockcomment(next);
    // Addressof.
    case '&': return lex_addressof(peek, next);
    // String, possibly multiline.
    case '"':
    case '\'':
      return lex_longstring(peek, next);
    // Token string.
    case ':': return lex_shortstring(peek, next);
    default: break;
  }

  // If we get here it's either a number or a word.
  // No support for negatives as yet, it makes distinguishing a negative number
  // and the word `-` tricky.
  if (isdigit(ch)) {
    return lex_number(peek, next);
  }

  return lex_word(peek, next);
}

void lex_linecomment(char (*peek)(void), char (*next)(void)) {
  while (peek() != '\n' && peek() != '\0') next();
}

void lex_blockcomment(char (*next)(void)) {
  int depth = 0;
  char ch;
  do {
    ch = next();
    if (ch == '(') ++depth;
    if (ch == ')') --depth;
  } while (depth);
}

// Read text into the stack until pred(peek()) is true.
// Null terminate it and return a pointer to the start of the string.
// Note that any subsequent use of the stack invalidates the string.
char* readuntil(char (*peek)(void), char (*next)(void), int (*pred)(int)) {
  char* buf = (char*)&STACK[STACKP];
  size_t len = 0;
  while (!pred(peek())) {
    buf[len++] = next();
  }
  buf[len] = '\0';
  return buf;
}

// Equivalent to readuntil(), except that it then allocates a buffer for the
// string, copies the string into it, and returns a pointer to the buffer.
char* readuntila(char (*peek)(void), char (*next)(void), int (*pred)(int)) {
  char* buf = readuntil(peek, next, pred);
  char* str = malloc(strlen(buf) + 1);
  strcpy(str, buf);
  return str;
}

char quote;
int isquote(int ch) { return ch == quote; }

void lex_longstring(char (*peek)(void), char (*next)(void)) {
  quote = next();
  compile_string(readuntila(peek, next, isquote));
  next();
}

void lex_shortstring(char (*peek)(void), char (*next)(void)) {
  next(); // discard leading :
  compile_string(readuntila(peek, next, isspace));
}

void lex_number(char (*peek)(void), char (*next)(void)) {
  char* buf = readuntil(peek, next, isspace);
  Cell num = strtol(buf, &buf, 0);
  if (*buf) {
    error("Invalid number.");
    return;
  }
  compile_number(num);
}

void lex_addressof(char (*peek)(void), char (*next)(void)) {
  next(); // Drop leading &
  char* name = readuntil(peek, next, isspace);
  Word* word = find_word(name);
  if (!word) {
    print("undefined: "); error(name);
    return;
  }
  compile_addressof(word);
}

void lex_word(char (*peek)(void), char (*next)(void)) {
  char* name = readuntil(peek, next, isspace);
  Word* word = find_word(name);
  if (!word) {
    print("undefined: "); error(name);
    return;
  }
  compile_word(word);
}
