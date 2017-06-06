SRCS=$(wildcard *.c)
HDRS=$(wildcard *.h)
WORDS=$(wildcard builtins/*.nf)

all: notforth

debug: notforth
	gdb -ex run notforth

run: notforth
	./notforth

clean:
	> builtins/all.c
	> builtins/all.h
	rm -f notforth notforth-bootstrap builtins/*.nf.impl builtins/*.nf.dict

notforth: ${SRCS} ${HDRS} builtins/all.c builtins/all.h
	gcc -DLINUX -Wall -Werror -g -O0 -o notforth ${SRCS}

builtins/all.c: $(WORDS:%.nf=%.nf.impl)
	echo "#ifdef ENABLE_BUILTINS" > builtins/all.c
	cat builtins/*.nf.impl >> builtins/all.c
	echo "#endif" >> builtins/all.c

builtins/all.h: $(WORDS:%.nf=%.nf.dict)
	echo "#ifdef ENABLE_BUILTINS" > builtins/all.h
	cat builtins/*.nf.dict >> builtins/all.h
	echo "#endif" >> builtins/all.h

builtins/%.nf.dict builtins/%.nf.impl: builtins/%.nf notforth-bootstrap
	(cd builtins && ../notforth-bootstrap <../$< )

notforth-bootstrap: ${SRCS} ${HDRS}
	gcc -DLINUX -Wall -Werror -g -O0 -o notforth-bootstrap ${SRCS}

.SUFFIXES:
