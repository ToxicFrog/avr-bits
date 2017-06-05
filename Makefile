all: notforth

SRCS=$(wildcard *.c)
# notforth.c dictionary.c repl.c tty.c corewords.c stacks.c
HDRS=$(wildcard *.h)

notforth: ${SRCS} ${HDRS}
	gcc -DLINUX -Wall -Werror -g -o notforth ${SRCS}

debug: notforth
	gdb -ex run notforth

run: notforth
	./notforth
