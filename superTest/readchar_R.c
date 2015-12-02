#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "readchar_R.h"

char ReadCharacter(int fd, int position){
	char buf;

	lseek(fd, position, SEEK_SET);
	read(fd, &buf, 1);
	//we could try checking for errors but we assume calling function already did it
	return buf;
}
