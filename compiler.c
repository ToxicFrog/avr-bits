#include "common.h"

#include <stdlib.h>
#include <string.h>

#include "compiler.h"
#include "dictionary.h"
#include "execute.h"
#include "error.h"

// Stack of Word definitions that we're compiling. When we finish a word, it
// gets popped from this stack and added to DICTIONARY, albeit without a name.
// It's up to the user to call defn to give it one, if they want.
Word* compiling = NULL;


#ifdef LINUX
void c_beginfn(Word*);
void c_endfn(Word*);
void c_pushstring(const char*);
void c_pushnumber(Cell);
void c_pushword(Word*);
void c_callword(Word*);
#else
#define c_beginfn(x)
#define c_endfn(x)
#define c_pushstring(x)
#define c_pushnumber(x)
#define c_pushword(x)
#define c_callword(x)
#endif

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

  // Pop it from the compilation stack.
  Word* word = compiling;
  compiling = word->next;

  // Copy the wordlist off the stack and restore old STACKP.
  // word->execute is the value of STACKP just before we started writing instructions.
  compile_eof();
  size_t len = STACKP - (size_t)word->execute;
  STACKP = (size_t)word->execute;
  Cell* body;
  CHECK_MALLOC(body, len * sizeof(Cell), "anonymous function body");
  memcpy(body, &STACK[STACKP], len * sizeof(Cell));
  word->execute = (WordImpl)body;

  // Link the word into the dictionary.
  word->next = DICTIONARY;
  DICTIONARY = word;

  compile_addressof(word);
}

// Functions for actually emitting bytecode. All bytecode goes on the stack.

void compile_string(const char * str) {
  push((Cell)OP_PUSHLITERAL);
  push((Cell)str);
  c_pushstring(str);
}

void compile_number(Cell num) {
  push((Cell)OP_PUSHLITERAL);
  push(num);
  c_pushnumber(num);
}

#include <stdio.h>

void compile_word(Word* word) {
  if (word->flags & IS_IMMEDIATE) {
    execute_word(word);
  } else if (word->flags & IS_CONSTANT) {
    push((Cell)OP_PUSHLITERAL);
    push((Cell)word->execute);
    c_pushnumber((Cell)word->execute);
  } else if (word->flags & IS_BYTECODE) {
    push((Cell)OP_CALLWORD);
    push((Cell)word->execute);
    c_callword(word);
  } else {
    push((Cell)word->execute);
    c_callword(word);
  }
}

void compile_addressof(Word* word) {
  push((Cell)OP_PUSHLITERAL);
  push((Cell)word);
  c_pushword(word);
}

void compile_eof() {
  push((Cell)OP_EOF);
}

// Called when something has gone wrong; the caller wants us to unwind the
// compilation stack and clean up after ourselves.
void compile_abort() {
  while (compiling) {
    Word* tmp = compiling;
    compiling = tmp->next;
    if (tmp->name) free((char*)tmp->name);
    free(tmp);
  }
}

#ifdef LINUX

// Code for compiling C words goes here.

#include <stdio.h>
#include <ctype.h>
#include <string.h>

const char * mangle(const char * name) {
  // The maximum possible length of a name -- assuming no shenanigans are happening
  // in C -- is the number of characters you can fit on the stack, x3 if they all
  // have to be X-escaped due to not being alphanumeric.
  static char mangled_name[STACKBYTES * 3];
  // Determine the mangled name
  char* mch = mangled_name;
  for (const char* ch = name; *ch; ++ch) {
    if (isalnum(*ch)) {
      *(mch++) = *ch;
    } else {
      *(mch++) = 'X';
      mch += sprintf(mch, "%X", *ch);
    }
  }
  *mch = '\0';
  return mangled_name;
}

// Each one has two parts, the impl and the header
// the impl has the form:
// #includes
// list of function definitions
// dictionary fragment definition
// the header has the form:
// extern declaration of dictionary fragment
// redefinition of LAST_DICT and LAST_DICT_LEN macros
FILE * cimpl = NULL;
FILE * cdict = NULL;
char cname[STACKBYTES];
size_t nrof_cdefs = 0;

void word_cfile() {
  char* name = (char*)pop();
  if (name) {
    CHECK(!cimpl,
      "Attempt to open %s.{dict,impl} when there's already a c/file open.",
      name);
    sprintf(cname, "%s.impl", name);
    cimpl = fopen(cname, "w");
    sprintf(cname, "%s.dict", name);
    cdict = fopen(cname, "w");

    strcpy(cname, mangle(name));

    fprintf(cdict, "// Words from %s\n\n", name);
    free(name);
  } else {
    CHECK(cimpl, "Attempt to close a c/file without one open.");
    // Finalize everything.

    Word* cdef = DICTIONARY;
    while (nrof_cdefs) {
      nrof_cdefs--;
      const char* mangled_name = mangle(cdef->name);
      fprintf(cdict, "  { (Word*)1, word_%s_impl, word_%s_name, SELF_IN_FLASH | NEXT_IN_FLASH | NAME_IN_FLASH | %d },\n",
        mangled_name, mangled_name, cdef->flags & ~IS_BYTECODE);
      do { cdef = next_word(cdef); } while(!cdef->name);
    }
    fclose(cdict); cdict = NULL;
    fclose(cimpl); cimpl = NULL;
  }
}

// Similar to defn, but assumes the top of the stack is a function full of string
// literals. Write those out to the C files selected with c/file and save the
// function itself as a no-op. The strings are freed.
void word_cdefn() {
  CHECK(cimpl, "Attempt to call c/impl before c/file.");
  char* body = (char*)pop();
  const char* name = (char*)pop();
  const char* mangled_name = mangle(name);

  // Write the implementation
  fprintf(cimpl, "\nconst PROGMEM char word_%s_name[] = \"%s\";\n",
    mangled_name, name);
  fprintf(cimpl, "void word_%s_impl() {\n%s\n}\n", mangled_name, body);
  free(body);

  // Store a no-op version in the dictionary. We can't call it without recompiling,
  // but this lets us compile references to it, so functions later in a file can
  // refer to functions defined earlier in it.
  register_word(name, NULL)->flags |= IS_BYTECODE;
  ++nrof_cdefs;
}

// Functions for compiling words written in notforth into C.

// How much space we allocate for a function definition.
#define C_IMPL_BUFSIZE (size_t)4096
#include <stdarg.h>

void c_append(const char * fmt, ...) {
  //printf("c_append: %s\n", fmt);
  if (!compiling || !cimpl) return;
  va_list ap;
  va_start(ap, fmt);
  vsprintf((char*)compiling->name + strlen(compiling->name), fmt, ap);
  va_end(ap);
  //printf("c_append: ok\n");
}

void c_beginfn(Word* word) {
  if (!compiling || !cimpl) return;
  CHECK_MALLOC(compiling->name, C_IMPL_BUFSIZE, "buffer for C source code");
  c_append("\nvoid word_anon_%p() {\n", word);
}

void c_endfn(Word* word) {
  if (!compiling || !cimpl) return;
  c_append("}\n");
  fprintf(cimpl, "%s", (char*)word->name);
  free((char*)word->name);
  word->name = NULL;
}

void c_register_word(Word* word) {
  if (!cimpl) return;
  const char* mangled_name = mangle(word->name);
  fprintf(cimpl, "#define word_%s_impl word_anon_%p\n",
    mangled_name, word);
  fprintf(cimpl, "const PROGMEM char word_%s_name[] = \"%s\";\n",
    mangled_name, word->name);
  // We force the IS_BYTECODE flag off here because once this definition gets
  // loaded at compile time, it isn't bytecode -- but this is probably getting
  // called from `defn`, which means the function *in memory* is implemented as
  // bytecode and that flag will be set.
  fprintf(cdict,
    "  { (Word*)1, word_%s_impl, word_%s_name, "
    "(SELF_IN_FLASH | NEXT_IN_FLASH | NAME_IN_FLASH | %d) & ~IS_BYTECODE },\n",
    mangled_name, mangled_name, word->flags);
}

void c_pushstring(const char* str) {
  c_append("  push((Cell)\"%s\");\n", str);
}

void c_pushnumber(Cell n) {
  c_append("  push(%d);\n", n);
}

void c_pushword(Word* word) {
  if (word->name) {
    c_append("  push((Cell)word_%s_impl);\n", mangle(word->name));
  } else {
    c_append("  push((Cell)word_anon_%p);\n", word);
  }
}

void c_callword(Word* word) {
  if (word->name) {
    c_append("  word_%s_impl();\n", mangle(word->name));
  } else {
    c_append("  word_anon_%p();\n", word);
  }
}


#endif  // LINUX
