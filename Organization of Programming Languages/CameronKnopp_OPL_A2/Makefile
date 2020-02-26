# Trivial makefile for the calculator scanner/parser.
# Depends on default (built-in) rules for C compilation.

# Note that rule for goal (parse) must be the first one in this file.

test: parse.o scan.o
	g++ -o test parse.o scan.o
	./test < primes.txt > primesOutput.txt
	./test < average.txt > averageOutput.txt
clean:
	rm *.o
	rm test
	rm primesOutput.txt
	rm averageOutput.txt

parse.o: scan.h
scan.o: scan.h
