test: parse.o scan.o
	g++ -o test parse.o scan.o
	./test < primes.txt > primesOutput.txt
	./test < average.txt > averageOutput.txt

parse.o: scan.h
scan.o: scan.h

clean:
	rm *.o *.gch
	rm test
	rm primesOutput.txt
	rm averageOutput.txt


