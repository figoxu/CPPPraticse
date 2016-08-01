CC = gcc 
CXX = g++
# additional flags
# -Wall: warnings
# -g: debugging
ADD_CFLAGS := -Wall -g
# combine the flags (so that CFLAGS/LDFLAGS from the command line
# still work).
CFLAGS  := $(ADD_CFLAGS) $(CFLAGS)
CPPFALGS := $(ADD_CFLAGS)
LDFLAGS := $(LDFLAGS)
targets = test
.PHONY: all
all: $(targets)
test: main.o fun.o
	$(CXX) $^ -o $@ $(LDFLAGS)
main.o: main.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@
fun.o: fun.c fun.h
	$(CC) $(CFLAGS) -c $< -o $@
.PHONY: clean
clean:
	$(RM) $(targets) *.o

