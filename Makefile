all: cshell.c
	gcc -g -o cshell cshell.c

hello: process.c
	gcc -o -g hello process.c

clean:
	-rm cshell
