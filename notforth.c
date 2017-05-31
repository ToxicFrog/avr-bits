// Notforth main file for HOST. See notforth.ino for TARGET.

#ifdef HOST_NOTFORTH

#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define false 0x00
#define true (~false)

#include "execute.h"

//#define debuglog printf
#define debuglog(...) ;

void eat_line() {
  while (getchar() != '\n');
}

char read_word(char ch) {
  // Check for reader macros
  if (ch == '\\') {
    // This is (someday) going to be one of:
    // \x<hex literal>
    // \d<decimal literal>
    // \w<word>
    // for now we just skip
    debuglog("Found reader macro: ");
    while (isprint(ch) && !isspace(ch)) {
      ch = getchar();
      debuglog("%c", ch);
    }
    debuglog("\n");
    return ch;
  }

  // Check for comments
  if (ch == '#') {
    // skip rest of line
    debuglog("found comment\n");
    eat_line();
    return '\n';
  }

  // check for decimal int literal
  if (isdigit(ch)) {
    int n = 0;
    for (; isdigit(ch); ch = getchar()) {
      n = n*10 + (ch - '0');
    }
    debuglog("found int: %d\n", n);
    push(n);
    return ch;
  }

  // read contents of word
  char name[32];
  int namelen = 0;

  for (; isprint(ch) && !isspace(ch); ch = getchar()) {
    name[namelen++] = ch;
    if (namelen >= 32) {
      // error! name too long
      printf("error! name too long\n");
      eat_line();
      return '\n';
    }
  }
  name[namelen] = '\0';
  debuglog("found word: %s\n", name);

  execute_word(find_word(name));

  return ch;
}

void read_line() {
  char ch = getchar();
  while (true) {
    // Skip leading whitespace
    while (isspace(ch)) {
      if (ch == '\n') return;
      ch = getchar();
    };

    ch = read_word(ch);
  }
}

#include "core.c"

int main() {
  printf("Pointer size: %lu\n", sizeof(void*));
  printf("Word size: %lu\n", sizeof(Word));
  printf("Offsets: 0x%lx 0x%lx 0x%lx 0x%lx\n",
    offsetof(Word, next), offsetof(Word, execute), offsetof(Word, flags),
    offsetof(Word, data));
  register_word("bye", word_bye);
  register_word(".", word_printnum);
  register_word("+", word_add);
  register_word("-", word_sub);
  register_word("*", word_mul);
  register_word("/", word_div);
  register_word("%", word_mod);
  register_word("?", word_peek);
  register_word("!", word_poke);
  register_word("words", word_words);
  while (true) {
    printf("] ");
    read_line();
  }
  return 0;
}

#endif  // HOST_NOTFORTH
