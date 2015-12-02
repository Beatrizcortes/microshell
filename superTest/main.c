#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "count_M.h"
#include "readchar_R.h"
#include <string.h>

int main (int argc, char *argv[]) {

	char character;
	int fd;
	int result=0;

	if(argc!=4) {
	//check if there are three arguments
		printf("Three arguments expected. Can't execute\n");
		return 1;
	}
	if(*argv[1]=='M' && *argv[1]=='R') {
	//check if first argument is not correct
		printf("First argument is invalid. Can't execute\n");
		return 1;
	}
	character = argv[3][0];//get first (and only) character of last argument
	//if there's more than one character it'll just ignore the rest
	fd = open(argv[2],O_RDONLY);
	//open file specified, read only. Will fail if it doesn't exist
	if(fd<0) {
		printf("Error opening file. Does it exist?\n");
		return 1;
	}
	if(*argv[1]=='M') {
		//Call countCharacters
		result = CountCharacters(fd, character);
		printf("The character has %d ocurrences\n", result);
	} else {
		//Call readCharacter and count yourself
		int counter;
		char auxchar;
		int eof = lseek(fd, 0, SEEK_END); //get end of file
		for(counter=0; counter<eof-1;counter++) {
			auxchar=ReadCharacter(fd, counter);
			if(auxchar==*argv[3])
				result++;
		}
		printf("The character has %d ocurrences\n", result);
	}
	close(fd);//it doesn't really matter if this one fails. We haven't written aything.
	return 0;
}
