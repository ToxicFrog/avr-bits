// Library for compiling functions into C code.
// See c_compiler.h for API details.

#ifdef LINUX

#include "common.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "c_compiler.h"
#include "dictionary.h"
#include "execute.h"
#include "error.h"
#include "compiler.h"

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

// cimpl is the C implementation file we're currently writing.
// It contains all the C code needed to implement the (C or notforth) functions
// being defined.
// It is conventionally named <filename>.impl -- not .c because then the Arduino
// IDE will autodetect it and things will go wrong.
FILE * cimpl = NULL;
char cname[STACKBYTES];

void word_cfile() {
  char* name = (char*)pop();
  if (name) {
    CHECK(!cimpl,
      "Attempt to open %s.impl when there's already a c/file open.",
      name);
    sprintf(cname, "%s.impl", name);
    cimpl = fopen(cname, "w");

    strcpy(cname, mangle(name));

    fprintf(cimpl, "\n//// Definitions from %s ////\n\n", name);
    free(name);
  } else {
    // Finalize everything.
    CHECK(cimpl, "Attempt to close a c/file without one open.");
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
  fprintf(cimpl, "void word_%s_impl() { %s }\n", mangled_name, body);
  free(body);

  // Store a no-op version in the dictionary. We can't call it without recompiling,
  // but this lets us compile references to it, so functions later in a file can
  // refer to functions defined earlier in it.
  c_register_word(register_word(name, NULL, IS_BYTECODE));
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
  ((char*)compiling->name)[0] = '\0';
  c_append("\nvoid word_anon_%p() {\n", word);
}

// End a function definition.
// In interpreter mode, the function is allocated an anonymous Word structure
// and that is pushed onto the stack.
// In compiler mode, no Word is allocated; the function body is written out and
// no other processing is done. The assumption is that the common case here is
// a subsequent call to `defn` that will allocate the Word and link it into the
// dictionary. In cases where the anonymous function needs to be pushed, it's
// up to the pusher (probably c_pushword) to create the Word ex nihilo.
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
  if (word->execute) {
    fprintf(cimpl, "#define word_%s_impl word_anon_%p\n",
      mangled_name, word);
  }
  fprintf(cimpl, "static const PROGMEM char word_%s_name[] = \"%s\";\n",
    mangled_name, word->name);
  // We force the IS_BYTECODE flag off here because once this definition gets
  // loaded at compile time, it isn't bytecode -- but this is probably getting
  // called from `defn`, which means the function *in memory* is implemented as
  // bytecode and that flag will be set.
  fprintf(cimpl,
    "static const PROGMEM Word word_%s_defn = {"
    " (Word*)&LAST_DEFINED_WORD, word_%s_impl, word_%s_name, SELF_IN_FLASH | NEXT_IN_FLASH | NAME_IN_FLASH | %d"
    " };\n"
    "#undef LAST_DEFINED_WORD\n"
    "#define LAST_DEFINED_WORD word_%s_defn\n\n",
    mangled_name, mangled_name, mangled_name, (word->flags & ~IS_BYTECODE), mangled_name);
}

void c_pushstring(const char* str) {
  c_append("  push((Cell)\"%s\");\n", str);
}

void c_pushnumber(Cell n) {
  c_append("  push(%d);\n", n);
}

// This is called by compile_addressof, which is given a Word and emits code to
// push it onto the stack. If the word is in ROM, it first calls register_word
// to copy it to RAM and pushes that. This happens before calling c_pushword.
// compile_addressof is called under two circumstances:
// - when compiling the @ prefix operator, and
// - when compiling a { function literal } to push the resulting anonymous function
void c_pushword(Word* word) {
  // c_append is a no-op outside of compilation state, but we also need to emit
  // some code directly here, so abort early.
  if (!compiling || !cimpl) return;

  // We assume that the word was compiled into ROM, since we're in compilation
  // mode. Note that if the function being pushed was defined (using `defn`) in
  // the same compilation unit, word->flags will claim it's in RAM, but once it
  // gets compiled in, this stops being the case.
  // For this reason, we don't trust the value of word->flags and just unconditionally
  // assume that by the time this generated code is being called, the word is in
  // ROM.

  if (word->name) {
    // Named word. The first time we're called, we allocate a buffer for a RAM
    // copy of the _defn and copy the definition from ROM. This slows down the
    // first call, but means the only memory impact is sizeof(Word*) unless the
    // function is actually called.
    // TODO: some sort of deduplication so that if multiple functions need a copy
    // in RAM of the same function, only one is allocated.
    const char* mangled_name = mangle(word->name);
    fprintf(cimpl,
      "#ifndef WORD_%s_TMP_DEFINED\n"
      "#define WORD_%s_TMP_DEFINED\n"
      "Word* word_%s_tmp = NULL;\n"
      "#endif\n",
      mangled_name, mangled_name, mangled_name);
    c_append(
      "  if (!word_%s_tmp) {\n"
      "    word_%s_tmp = malloc(sizeof(Word));\n"
      "    memcpy_P(word_%s_tmp, &word_%s_defn, sizeof(Word));\n"
      "  }\n"
      "  push((Cell)word_%s_tmp);\n",
      mangled_name, mangled_name, mangled_name, mangled_name, mangled_name, mangled_name);

  } else {
    // Anonymous word. This is called at the end of every {...} block, but if
    // it is a top-level block, `compiling` will be NULL and c_append is a no-op.
    // Inside a function, we need to create and push an anonymous Word for it.
    fprintf(cimpl,
      "#ifndef WORD_%p_TMP_DEFINED\n"
      "#define WORD_%p_TMP_DEFINED\n"
      "Word* word_%p_tmp = NULL;\n"
      "#endif\n",
      word, word, word);
    c_append(
      "  if (!word_%p_tmp) {\n"
      "    word_%p_tmp = calloc(1, sizeof(Word));\n"
      "    word_%p_tmp->execute = (WordImpl)word_anon_%p;\n"
      "  }\n"
      "  push((Cell)word_%p_tmp);\n",
      word, word, word, word, word);
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
