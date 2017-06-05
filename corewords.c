// Core words for notforth.

#include "config.h"

#include "dictionary.h"
#include "tty.h"
#include "stacks.h"

//// IO words ////

void word_printnum() {
  printint(pop()); println("");
}

void word_printstr() {
  println((const char*)pop());
}

void word_stack() {
  print("Stack size: "); printint(STACKP); println("");
  for (int i = STACKP-1; i >= 0; --i) {
    printint(STACK[i]); println("");
  }
  println("----");
}

//// Math words ////

void word_add() {
  int x = pop(), y = pop();
  push(x+y);
}

void word_sub() {
  int x = pop(), y = pop();
  push(x-y);
}

void word_mul() {
  int x = pop(), y = pop();
  push(x*y);
}

void word_div() {
  int x = pop(), y = pop();
  push(x/y);
}

void word_mod() {
  int x = pop(), y = pop();
  push(x%y);
}

//// Memory words ////

void word_peek() {
  int* ptr = (int*)((uint64_t)pop());
  push(*ptr);
}

void word_poke() {
  int* ptr = (int*)((uint64_t)pop());
  int val = pop();
  *ptr = val;
}

void word_dup() {
  push(peek());
}

//// Control words ////

void printstr_P(const char * str) {
  char buf[32]; // FIXME
  strcpy_P(buf, str);
  print(buf);
}

void printname(const Word* word) {
  if (!word->name) {
    print("(anonymous)");
    return;
  }
  if (word->flags & NAME_IN_FLASH) {
    printstr_P(word->name);
  } else {
    print(word->name);
  }
}

void word_words() {
  for (Word* word = DICTIONARY; word; word = next_word(word)) {
    printint((intptr_t)word); print(":"); printint((intptr_t)word->execute);
    print(" ");
    printname(word);
    print(" ["); printint(word->flags); println("]");
  }
}

void word_const() {
  const WordImpl value = (WordImpl)pop();
  // Constants are IMMEDIATE so that executing them during a function definition
  // embeds the constant in the generated function using PUSHLITERAL.
  register_word((const char*)pop(), value)->flags |= IS_CONSTANT | IS_IMMEDIATE;
}

void impl_to_wordname(WordImpl impl) {
  for (Word* word = DICTIONARY; word; word = next_word(word)) {
    if (word->execute == impl) {
      print(" :"); printname(word);
      return;
    }
  }
}

void word_list() {
  Word* word = (Word*)pop();
  print("# word "); printname(word); println("");
  if (word->flags & IS_CONSTANT) {
    print("constant "); printint((intptr_t)word->execute); println("");
  } else if (word->flags & IS_WORDLIST) {
    WordImpl* op = (WordImpl*)word->execute;
    while (*op != NULL) {
      if (*op == (WordImpl)0x0001) {
        print("pushliteral "); printint((intptr_t)*(++op)); println("");
      } else if (*op == (WordImpl)0x0002) {
        print("call word@"); printint((intptr_t)*(++op)); print(" "); printname((Word*)*op); println("");
      } else {
        print("call C@"); printint((intptr_t)*op); impl_to_wordname(*op); println("");
      }
      ++op;
    }
  } else {
    print("call C@"); printint((intptr_t)word->execute); println("");
  }
  println("# end");
}

#ifdef LINUX
void word_bye() {
  exit(0);
}
#endif

// the { that begins a function definition
// switches the core into compile mode, if it's not already there, and pushes
// a new, empty wordlist function onto the definition stack (can this be the same as the data stack?)
void word_beginfn() {
  defn_begin();
}

// the } that ends a function definition
// switches the core out of compile mode, if this is the outermost function we're defining
// allocates a Word for the function and pushes it onto the stack
// The Word is added to the dictionary so it can be freed later, but has no name
// To give it a name, use defn once it's on the stack.
void word_endfn() {
  push((Cell)defn_end());
}

void word_defn() {
  Word* fn = (Word*)pop();
  char* name = (char*)pop();
  fn->name = name;
}

const PROGMEM char word_const_name[] = "const";

const PROGMEM Word CORE_WORDS[] = {
  { (Word*)1, word_printnum, ".", SELF_IN_FLASH | NEXT_IN_FLASH },
  { (Word*)1, word_printstr, "s.", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)1, word_stack, ".s", NEXT_IN_FLASH | SELF_IN_FLASH },

  { (Word*)1, word_beginfn, "{", NEXT_IN_FLASH | SELF_IN_FLASH | IS_IMMEDIATE },
  { (Word*)1, word_endfn, "}", NEXT_IN_FLASH | SELF_IN_FLASH | IS_IMMEDIATE},
  { (Word*)1, word_defn, "defn", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)1, word_const, word_const_name, NEXT_IN_FLASH | SELF_IN_FLASH | NAME_IN_FLASH },

  { (Word*)1, word_dup, "dup", NEXT_IN_FLASH | SELF_IN_FLASH },

  { (Word*)1, word_add, "+", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)1, word_sub, "-", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)1, word_mul, "*", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)1, word_div, "/", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)1, word_mod, "%", NEXT_IN_FLASH | SELF_IN_FLASH },

  { (Word*)1, word_peek, "?", NEXT_IN_FLASH | SELF_IN_FLASH },
  { (Word*)1, word_poke, "!", NEXT_IN_FLASH | SELF_IN_FLASH },

#ifdef LINUX
  { (Word*)1, word_bye, "bye", NEXT_IN_FLASH | SELF_IN_FLASH },
#endif

  { NULL, word_list, "list", SELF_IN_FLASH },
};

void load_core_words() {
  DICTIONARY = (Word*)CORE_WORDS;  // cast to remove the const
  register_word("words", word_words)->flags |= NEXT_IN_FLASH;
}
