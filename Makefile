SRCS=$(wildcard *.c)
HDRS=$(wildcard *.h)
WORDS=$(wildcard builtins/*.nf)

all: notforth

debug: notforth
	gdb -ex run notforth

run: notforth
	./notforth

clean:
	> builtins/all.dict
	> builtins/all.impl
	rm -f notforth notforth-bootstrap builtins/*.nf.impl builtins/*.nf.dict

notforth: ${SRCS} ${HDRS} builtins/all.impl builtins/all.dict
	gcc -DLINUX -Wall -Werror -g -O0 -o notforth ${SRCS}

builtins/all.impl: $(WORDS:%.nf=%.nf.impl)
	cat builtins/*.nf.impl > builtins/all.impl

builtins/all.dict: $(WORDS:%.nf=%.nf.dict)
	cat builtins/*.nf.dict > builtins/all.dict

builtins/%.nf.dict builtins/%.nf.impl: builtins/%.nf notforth-bootstrap
	(cd builtins && ../notforth-bootstrap <../$< )

notforth-bootstrap: ${SRCS} ${HDRS}
	gcc -DLINUX -DNO_BUILTINS -Wall -Werror -g -O0 -o notforth-bootstrap ${SRCS}
