## This is a simple Makefile 

# Define what compiler to use and the flags.
CC=cc
CXX=g++
CCFLAGS= -g -std=c++11 -Wall -Werror
LDLIBS= -lm
SRC=$(wildcard *.cpp)

all: proj



###################################
# BEGIN SOLUTION
proj: $(SRC)
	$(CXX) -o proj $^ $(CCFLAGS) $(LDLIBS)


clean:
	rm -f *.o proj