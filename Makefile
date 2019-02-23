MICRONUCLEUS=mn-cli/micronucleus

all: ${MICRONUCLEUS}

${MICRONUCLEUS}:
	make -C micronucleus/commandline
