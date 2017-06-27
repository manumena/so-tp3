.PHONY : all clean run new

MPIROOT = /usr

CFLAGS+= -Wall -g $(INCL)
CC=gcc
MPICC=  $(MPIROOT)/bin/mpic++
INCL= -I$(MPIROOT)/include
SRCS= consola.cpp nodo.cpp HashMap.cpp
TEST_SRCS= test.cpp nodo.cpp HashMap.cpp
TEST_BIN= test
BIN= dist_hashmap

all: dist_hashmap

$(BIN): $(SRCS)
	$(MPICC) $(CFLAGS) -o $(BIN) $(SRCS)

$(TEST_BIN): $(TEST_SRCS)
	$(MPICC) $(CFLAGS) -o $(TEST_BIN) $(TEST_SRCS)


clean:
	rm -f $(BIN) $(TEST_BIN)

new: clean all
