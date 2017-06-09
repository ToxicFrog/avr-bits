SRCS=$(wildcard *.c)
HDRS=$(wildcard *.h)
WORDS=$(wildcard builtins/*.nf)
CC=gcc -std=c99 -Wall -Werror -g -O0

all: notforth

test: notforth
	./notforth < TEST

debug: notforth
	gdb -ex run notforth

run: notforth
	./notforth

clean:
	> builtins/all.c
	> builtins/all.h
	rm -f notforth notforth-bootstrap builtins/*.nf.impl builtins/*.nf.dict

notforth: ${SRCS} ${HDRS} builtins/all.c builtins/all.h
	${CC} -DLINUX -o notforth ${SRCS}

builtins/all.c: $(WORDS:%.nf=%.nf.impl)
	(echo "#ifdef ENABLE_BUILTINS"; cat builtins/*.nf.impl; echo "#endif") > builtins/all.c

builtins/all.h: $(WORDS:%.nf=%.nf.dict)
	(echo "#ifdef ENABLE_BUILTINS"; cat builtins/*.nf.dict; echo "#endif") > builtins/all.h

builtins/%.nf.dict builtins/%.nf.impl: builtins/%.nf notforth-bootstrap
	(cd builtins && ../notforth-bootstrap) <$< # >/dev/null

notforth-bootstrap: ${SRCS} ${HDRS}
	${CC} -DLINUX -o notforth-bootstrap ${SRCS}

.SUFFIXES:
