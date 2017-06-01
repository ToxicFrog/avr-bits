all: notforth

SRCS=notforth.c dictionary.c repl.c tty.c

notforth: ${SRCS}
	gcc -DHOST_NOTFORTH -Wall -Werror -g -o notforth ${SRCS}

debug: notforth
	gdb -ex run notforth

run: notforth
	./notforth
