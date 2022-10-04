
all: program1

program1: wordle.c
	gcc -g -Wall -pedantic -o wordle -I wordle.c 
