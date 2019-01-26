#include "common.h"

#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "c_compiler.h"
#include "dictionary.h"
#include "execute.h"
#include "error.h"

// Stack of Word definitions that we're compiling. When we finish a word, it
// gets popped from this stack and added to DICTIONARY, albeit without a name.
// It's up to the user to call defn to give it one, if they want.
Word* compiling = NULL;

// Create a new, blank Word definition and put it on the compilation stack.
void word_beginfn() {
  Word* word;
  CHECK_MALLOC(word, sizeof(Word), "anonymous function dictionary entry");
  word->next = compiling;
  word->execute = (WordImpl)STACKP; // save stack pointer so we can figure out where the bytecode starts later
  word->name = NULL;
  word->flags = IS_BYTECODE;
  compiling = word;
  c_beginfn(word);
}

// Finalize a word definition: pop all bytecode on the data stack back to the
// last {, copy it into the Word on top of the compilation stack, then link
// the Word into the dictionary without a name and push it onto the data stack.
void word_endfn() {
  CHECK(compiling, "} with no matching {");

  c_endfn(compiling);

  // Copy the wordlist off the stack and restore old STACKP.
  // word->execute is the value of STACKP just before we started writing instructions.
  compile_eof();
  size_t len = STACKP - (size_t)compiling->execute;
  STACKP = (size_t)compiling->execute;
  Cell* body;
  CHECK_MALLOC(body, len * sizeof(Cell), "anonymous function body");
  memcpy(body, &STACK[STACKP], len * sizeof(Cell));
  compiling->execute = (WordImpl)body;

  // Pop it from the compilation stack.
  Word* word = compiling;
  compiling = word->next;

  // Link the word into the dictionary.
  word->next = DICTIONARY;
  DICTIONARY = word;

  // Push the address of the word onto the stack.
  compile_addressof(word);
}

// Functions for actually emitting bytecode. All bytecode goes on the stack.

void op_pushliteral(Cell val) {
  if (compiling) {
    push((Cell)OP_PUSHLITERAL);
    push(val);
  } else {
    push(val);
  }
}

void compile_string(const char * str) {
  op_pushliteral((Cell)str);
  c_pushstring(str);
}

void compile_number(Cell num) {
  op_pushliteral(num);
  c_pushnumber(num);
}

void compile_word(Word* word) {
  if (!compiling || word->flags & IS_IMMEDIATE) {
    execute_word(word);
  } else if (word->flags & IS_CONSTANT) {
    compile_number((Cell)word->execute);
  } else if (word->flags & IS_BYTECODE) {
    push((Cell)OP_CALLWORD);
    push((Cell)word->execute);
    c_callword(word);
  } else {
    push((Cell)word->execute);
    c_callword(word);
  }
}

// Called by lex_addressof, which handles tokens of the form @foo.
// By the time this is called, word lookup (returning the Word* for foo) has
// already succeeded. Our job is to emit code pushing the address of the Word.
void compile_addressof(Word* word) {
  // If the word is in ROM, we create a copy of the dictionary entry in RAM and
  // push that -- it'll take precedence over the version in ROM from now on.
  // Otherwise we end up pushing the address of the temporary buffer used by
  // find_word, which is invalidated next time it gets called.
  if (word->flags & SELF_IN_FLASH) {
    word = register_word(word->name, word->execute, word->flags & ~(SELF_IN_FLASH|NEXT_IN_FLASH));
  }
  op_pushliteral((Cell)word);
  c_pushword(word);
}

void compile_eof() {
  if (compiling) push((Cell)OP_EOF);
}

// Called when something has gone wrong; the caller wants us to unwind the
// compilation stack and clean up after ourselves.
// If not currently compiling, this is a no-op.
void compile_abort() {
  while (compiling) {
    Word* tmp = compiling;
    STACKP = (size_t)tmp->execute;
    compiling = tmp->next;
    if (tmp->name) free((char*)tmp->name);
    free(tmp);
  }
}

