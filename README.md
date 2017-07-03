# notforth -- a stack-based language for the AVR

`notforth` is an itty bitty language for the AVR. It's Forth-inspired, especially under the hood, but also draws heavily on postscript.

At the moment it depends on the Arduino libraries for serial access, but this is not a hard dependency; you can compile an arduino-less version if you're willing to give up the serial REPL.

## Runtime features

 * Runs on both amd64 Linux and AVR
 * REPL (stdin/stdout on Linux, UART0 on AVR)
 * Functions can be compiled on the host to be stored in flash on the AVR
 * Small memory footprint (<200 bytes)
 * Features (such as the REPL) can be selectively omitted to converse flash
 * Sublime Text syntax hilighting rules (in `share/`)

## Building

Notforth can be built for both Linux and Arduino (non-Arduino AVR compatibility is in the works). Most of the notforth standard library is itself written in notforth, so before the Arduino version can be built, the Linux version needs to be used to bootstrap it.

### Configuration

Compile-time configuration settings are all in `config.h`, and are documented there.

### Linux build

Just use `make`. As long as you have `make`, `gcc`, and `bash`, this will build a minimal `nf-bootstrap`, use that to build the standard library, and then build the main `notforth` binary.

This will also generate the `builtins/all.h` and `builtins/all.c` files which the Arduino build will pick up.

### Arduino build

*After* building the linux version, just load the `notforth` directory in the Arduino IDE as a normal project and build it with `ctrl-R`.

Doing this before building the linux version will work, but the generated image will be missing most of the standard library and not terribly useful.

## Running

*Linux*: just run `./notforth` to get a REPL, or `./notforth <list of files>` to load and run each file in turn.

*Arduino*: upload the image with `ctrl-U` (or export the ihex and use `avrdude`), then reset the board and connect to it over the serial console at 9600 8n1. (There's currently no way to define a `main()` for notforth; this is planned.)

## The language

Notforth is a stack-based language with very little syntax. It has three stacks, one explicit (the data stack) and two implicit (the compilation stack and the execution stack); the data stack is used not only for function inputs and outputs, but also as scratch space for reading input and compiling function definitions into bytecode (the compilation stack holds metadata about the functions being compiled). All such data is safely copied off the stack before execution, so you don't need to worry about a function overwriting its own code, but you do need to be aware that having lots of stuff on the stack will reduce the amount of space available for entering long strings or lengthy function definitions at the REPL.

Tokens are separated by whitespace, and can be separated into three types:
- *Literals*, such as strings, numbers, and function definitions; when evaluated, these are simply pushed onto the stack.
- *Functions*; when evaluated, these do something, typically reading input from the stack and pushing output onto it. A typical example is `exch`, which swaps the top two values on the stack.
- *Macros*; these cannot be evaluated normally, but instead, are called *at compile time* as soon as the lexer finishes reading them. They can emit arbitrary bytecode (or modify bytecode that has already been emitted). `{` and `}`, which begin and end a function definition, are implemented as macros.

Internally, functions and macros can be implemented in either C or notforth, but from a caller's point of view they behave identically. Functions are looked up by name *at compile time* in a linked list of all defined functions, the *dictionary*; if multiple definitions with the same name exist, the most recent one takes precedence.

Note that once a value is pushed onto the stack, *no type information of any kind is associated with it*. Notforth has no idea whether the value on top of the stack is a number, a pointer to a string constant, the address of a memory-mapped IO register, or something else entirely. It's up to you to keep track of your stack and make sure that each function pops what it expects.

It is also important to remember that there is no garbage collection or concept of scope. If you push a pointer to `malloc`d space onto the stack, and you later pop it without `free`ing it, there is no way to reclaim that memory.

### Comments

Comments start with `#` (and run to the end of the line) or `(` (and run to the matching `)`. The latter form can be nested.

### Numbers

Numbers consist of a run of base-10 digits, or `0x` followed by a run of base-16 digits. There is currently no support for entering negative numbers (although you can create them on the stack with math).

### Strings

Short strings start with `:` and cannot contain whitespace. Long strings start with `'` or `"` and run to the matching quote; they may contain any whitespace, including newlines.

When read, the string is temporarily buffered on the stack, then copied into a freshly `malloc`d buffer; a pointer to this buffer is then pushed onto the stack. If you are not planning to pass the string to a function that will take ownership of it, don't forget to free it when you're done with it.

### Function values

By prefixing the name of a function with `@`, you can push the address of the function onto the stack rather than calling it. You can then call it later with `exec`, or do things like pass it to another function or to a flow-control construct like `if` or `while`.

*Warning*: `@` is currently not safe to use on words stored in flash (i.e. builtins). It will appear to work, but do the wrong thing. Don't go there. TODO: when using `@` on a word with `SELF_IN_FLASH` set, create a copy of the word definition at the head of the dictionary with `SELF_IN_FLASH` unset but all other fields intact, then push the address of that copy.

### Function definitions

Function definitions start with `{` (which is a macro implemented in C, not part of the lexer) and end with a corresponding `}`; they can be nested arbitrarily. When evaluated, this results in a pointer to the (anonymous) function definition being pushed, similar to the use of `@` on named functions. To associate a name with an anonymous function, use `defn` or `defmacro`.

## The standard library

Most of it is defined in `builtins/*.nf`. The documentation currently consists of the comments in those files (which I've tried to make informative).

The exceptions -- functions written in C and built into the core interpreter -- are documented here. Following the convention used for forth and postscript, function signatures are written as `<arguments on stack> <name of function> -- <outputs on stack`.

### `{` and `}`

These macros begin and end a function definition. See *Function Definitions* above.

### `c/file` and `c/defn`

`name c/file --`
`name body c/defn --`

These are used to generate code for functions and macros implemented in C but defined in notforth; `builtins/` contains a number of examples.

### `words `

`words --`

List all functions in the dictionary, most recent to least (i.e. functions listed earlier will take precedence over those listed later), in the format: `<address of dictionary entry>:<address of code> [<flags>] <name of function>`.

Most of this information is only useful for debugging notforth itself; note, furthermore, that the "address of dictionary entry" will be the same for every word stored in flash, as it instead points to a temporary buffer in RAM that the word is copied to for inspection.

### `list`

`fn list --`

Pops a function pointer from the stack and prints to the terminal the decompiled bytecode of the function. For functions not stored as bytecode (which includes all builtin functions; even the ones defined in notforth get compiled to C for easy storage in flash) it simply prints the address of the function's object code.

## TODO

 * Basic register/IO library
 * Example program on the arduino
 * Rewrite REPL in notforth; ability to compile in a `main` function
 * String/function interning; this will make compilation slower, but at runtime, who cares?
 * Support for negative numbers
 * Autogenerate documentation
