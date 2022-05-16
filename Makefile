countprimes: countprimes.c
	mpicc -g -Wall -o countprimes countprimes.c -lm

clean:
	rm -f *.o
	rm -f countprimes