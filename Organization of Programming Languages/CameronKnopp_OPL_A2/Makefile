# Trivial makefile for the calculator scanner/parser.
# Depends on default (built-in) rules for C compilation.

# Note that rule for goal (parse) must be the first one in this file.

parse: parse.o scan.o
	gcc -o parse scan.o parse.o

parse.o: scan.hpp parse.cpp
	gcc -c scan.hpp parse.cpp

scan.o: scan.hpp scan.cpp
	gcc -c scan.hpp scan.cpp

clean:
	rm *.o *.hpp.gch parse

