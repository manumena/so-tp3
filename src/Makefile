.PHONY : all clean run new

MPIROOT = /usr

CFLAGS+= -Wall -g $(INCL)
CC=gcc
MPICC=  $(MPIROOT)/bin/mpic++
INCL= -I$(MPIROOT)/include
SRCS= main.cpp consola.cpp nodo.cpp HashMap.cpp
TEST_SRCS= test.cpp consola.cpp nodo.cpp HashMap.cpp
TEST= test
BIN= dist_hashmap

all: dist_hashmap

$(BIN): $(SRCS)
	$(MPICC) $(CFLAGS) -o $(BIN) $(SRCS)

test: $(TEST_SRCS)
	$(MPICC) $(CFLAGS) -o $(TEST) $(TEST_SRCS)


clean:
	rm -f $(BIN) $(TEST)

new: clean all
