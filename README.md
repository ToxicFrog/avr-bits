# notforth -- a stack-based language for the AVR

`notforth` is an itty bitty language for the AVR. It's Forth-inspired, especially under the hood, but also draws heavily on postscript.

At the moment it depends on the Arduino libraries for serial access, but this is not a hard dependency; you can compile an arduino-less version if you're willing to give up the serial REPL

## Runtime features

 * Runs on both amd64 Linux and AVR
 * REPL (stdin/stdout on Linux, UART0 on AVR)
 * Functions can be compiled on the host to be stored in flash on the AVR
 * Small! <200b RAM, <2k flash if you omit the REPL

## Language features

 * Decimal and hex literals
 * Dynamically allocated string literals
 * Anonymous functions

## Building

Out of the box, most of the builtin functions are themselves written in notforth, and need to be ahead-of-time compiled by notforth before you can use them.

Running `make` on the host machine will automatically build a minimal notforth bootstrap program, use that to build the rest of the functions, and then compile and link the full version.

Once this is done, builtins/all.h and builtins/all.c contain the compiled versions of the builtin functions; they're automatically included (with appropriate `#define`s) by `corewords.c`. At this point you can simply feed the entire directory to the Arduino IDE, or compile all the top-level .c files using `avr-gcc`.

## TODO

 * Improved bootstrap process that supports successive files
 * Document the actual language
 * Error handling
 * Basic register/IO library
 * Example program on the arduino
 * Rewrite REPL in notforth; ability to compile in a `main` function
 * Replace Arduino serial library with notforth and/or C one
