#pragma once

#include "dictionary.h"
#include "execute.h"

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
