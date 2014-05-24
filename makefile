all: main.c
	mkdir -p bin
	gcc -o bin/primes main.c -Wall -pthread -lm
