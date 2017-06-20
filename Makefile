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
	rm -f notforth nf-bootstrap builtins/*.nf.impl builtins/*.nf.dict

notforth: ${SRCS} ${HDRS} builtins/all.c builtins/all.h
	${CC} -DLINUX -o notforth ${SRCS}


# 	(echo "#ifdef ENABLE_BUILTINS"; cat builtins/*.nf.impl; echo "#endif") > builtins/all.c

# 	(echo "#ifdef ENABLE_BUILTINS"; cat builtins/*.nf.dict; echo "#endif") > builtins/all.h

# builtins/all.c: $(WORDS:%.nf=%.nf.impl)
# builtins/all.h: $(WORDS:%.nf=%.nf.dict)
#builtins/%.nf.dict builtins/%.nf.impl: builtins/%.nf nf-bootstrap builtins.sh
builtins/all.h builtins/all.c: nf-bootstrap builtins.sh ${WORDS}
	bash builtins.sh

# This secretly depends on builtins/all.{h,c}, in that it incorporates them
# into the build, but it does not *require* them, and including that dependency
# explictly would make the build graph cyclical.
nf-bootstrap: ${SRCS} ${HDRS}
	touch builtins/all.h builtins/all.c
	${CC} -DLINUX -o nf-bootstrap ${SRCS}

.SUFFIXES:
