#include <ctype.h>  // isspace, etc

#include "repl.h"
#include "dictionary.h"
#include "tty.h"

#define debuglog(...)

void eat_line(char ch) {
  for (; ch != '\n'; ch = next_char()) {}
}

char process_word(char ch) {
  // Check for reader macros
  if (ch == '\\') {
    // This is (someday) going to be one of:
    // \x<hex literal>
    // \d<decimal literal>
    // \w<word>
    // for now we just skip
    debuglog("Found reader macro: ");
    while (isprint(ch) && !isspace(ch)) {
      ch = next_char();
      debuglog("%c", ch);
    }
    debuglog("\n");
    return ch;
  }

  // Check for comments
  if (ch == '#') {
    // skip rest of line
    debuglog("found comment\n");
    eat_line(ch);
    return '\n';
  }

  // check for decimal int literal
  if (isdigit(ch)) {
    int n = 0;
    for (; isdigit(ch); ch = next_char()) {
      n = n*10 + (ch - '0');
    }
    debuglog("found int: %d\n", n);
    //push(n);
    return ch;
  }

  // read contents of word
  char name[32];
  int namelen = 0;

  for (; isprint(ch) && !isspace(ch); ch = next_char()) {
    name[namelen++] = ch;
    if (namelen >= 32) {
      // error! name too long
      error("Name too long");
      eat_line(ch);
      return '\n';
    }
  }
  name[namelen] = '\0';

  Word* word = find_word(name);
  if (!word) {
    error("Word not found");
    eat_line(ch);
    return '\n';
  }
  execute_word(word);

  return ch;
}

void process_line() {
  char ch = next_char();
  while (1) {
    // Skip leading whitespace
    while (isspace(ch)) {
      if (ch == '\n') return;
      ch = next_char();
    };

    ch = process_word(ch);
  }
}

