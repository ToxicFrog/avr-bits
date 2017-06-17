#pragma once

// Enable runtime safety checks for things like stack over/underflow and malloc
// failures. Small code size/runtime performance impact.
// If enabled, these errors will result in a message being printed and a longjmp
// back to the last known safe state. If disabled, these errors are completely
// unchecked and, if they occur, will (at best) crash the program/reset the AVR.
#define SAFETY_CHECKS

// Size of data stack, in machine words (== 2 bytes each on AVR, 8 bytes on amd64).
// This is used not just for values explicitly put on the stack, but also as a
// scratch buffer for string/number input and for compilation, so this imposes a
// limit on maximum string literal length and maximum (non-precompiled) function
// length as well.
// We give it lots of extra stack on Linux for the bootstrapping process.
#define LINUX_STACKSIZE 1024
#define AVR_STACKSIZE 32

// Whether to enable the serial REPL for the Arduino build.
// Turning this off disables the ability to run code by typing it into the
// serial port, but also saves about 350b of RAM and 6k of flash.
#define SERIAL_REPL
