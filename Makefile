all: cshell.c
	gcc -o cshell cshell.c

hello: process.c
	gcc -o hello process.c

clean:
	-rm cshell.o
