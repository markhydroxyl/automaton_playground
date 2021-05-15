# regex makefile
CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -std=gnu11

CXX = g++
CXXFLAGS = -D__USE_POSIX -g -Wall -Wextra -pedantic -std=gnu++11

%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

all : regex

regex : regex.o graph_nfa.o array_nfa.o
	$(CXX) -o $@ $^

clean :
	rm -f *.o regex
