all: microshell

microshell: parser.c internal_commands.c microshell.c
	gcc -g -Wall microshell.c parser.c internal_commands.c -o microshell
clean:
	rm -f microshell
	rm -f *.o
