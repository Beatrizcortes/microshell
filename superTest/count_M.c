#include <stdio.h>
#include "count_M.h"
#include <sys/mman.h>
#include <unistd.h>
int CountCharacters(int fd, char character){
	char* mapPos;
	size_t nbytes;
	size_t counter;
	int coincidences=0;
	nbytes = lseek(fd, 0, SEEK_END);//get end pos of file	
	mapPos = mmap(0, nbytes, PROT_READ, MAP_SHARED, fd, 0);
	for (counter=0; counter<nbytes;counter++){
		if(mapPos[counter]==character)
			coincidences++;	
	}
	munmap(mapPos, nbytes);
	return coincidences;
}
