all: prog

prog: animal.o
	gcc -g -Wall -std=c99 -o prog animal.o

animal.o: animal.c
	gcc -g -Wall -std=c99 -c animal.c

clean:
	rm animal.o
	rm prog