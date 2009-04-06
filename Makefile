CC=gcc
CFLAGS=-Wall -g -ansi -pedantic
LDFLAGS=-pthread

OUTPUT=camp
SRC=camp4.c cli.c

DOXYGEN=doxygen
DOXYCONF=doxygen.conf
DOXYDIR=${PWD}/doc

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC) -o $(OUTPUT)

#doc:
#	$(DOXYGEN) $(DOXYCONF)

clean:
	rm -fr $(OUTPUT) $(DOXYDIR)
