/*
    internal_commands.c
*/
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>

#include "parser.h"
#include "internal_commands.h"

void execute_internal_command(command * C, char inid[])
{
//Gets command and chooses the correct function
//All names are "my"+command
	if(strcmp(C->argv[0],"mypwd")==0)
	    mypwd(C);
	    else if(strcmp(C->argv[0],"mymkdir")==0)
		mymkdir(C);
		else if(strcmp(C->argv[0],"myrmdir")==0)
		    myrmdir(C);
		    else if(strcmp(C->argv[0],"mycd")==0)
			mycd(C,inid);
			else if(strcmp(C->argv[0],"mycp")==0)
			    mycp(C);
			    else if(strcmp(C->argv[0],"myrm")==0)
				myrm(C);
				else if(strcmp(C->argv[0],"mycat")==0)
				    mycat(C);
				    else if(strcmp(C->argv[0],"myls")==0)
					myls(C,inid);
					else
			    		    printf("Unknown command\n");
}

void mypwd()
{
//Displays current directory to the user
	char buf[200];
	char* cwd;

	cwd = getcwd(buf, 200); //should be long enough
	if(cwd==NULL)
		printf("Cannot display the directory\n"); 
	else
		printf("%s\n", cwd);
}

void mymkdir(command *C)
{
//creates a new folder in the current directory
	int status;
	if(C->argv[1]!=NULL)
	{
		//Give permissions to everyone just in case
		status = mkdir(C->argv[1], S_IRWXU | S_IRWXG | S_IRWXO);
		if(status==-1)
			printf("Error creating folder\n");
		else
			printf("Done creating folder \n");
	}
	else//argument for name wasn't given
		printf("There's no name for the new folder.\n");
}

void myrmdir(command *C)
{
//Deletes the given folder, assuming it's in current directory
	int status;
	if(C->argv[1]!=NULL) //check that there's a folder name
	{
		status = rmdir(C->argv[1]);
		if(status==-1)
			printf("Error deleting the folder\n");
		else
			printf("Done deleting the folder\n");
	}
	else
		printf("You have not specified a folder\n");
}

void mycd(command *C, char inid[])
{
//Changes working directory to argumet. If no argument is given,
//it'll return to initial working directory
	int status;

	if(C->argv[1]==NULL) //if there's no argument...
	{
		status = chdir(inid);//go to initial
	} 
	else//go to specified folder. Works for relative and absolute
	{
		status = chdir(C->argv[1]);
	}
	if(status==-1)
	{
		printf("Folder couldn't be changed\n");
	}
	else
		printf("Folder changed\n");
}

void mycp(command * C)
{
//Copies first argument's file contents into second file's
//If the second file doesn't exist, it's created
	if(C->argv[1]==NULL||C->argv[2]==NULL)
		printf("You need to specify a source file and a target file\n");
	else
	{
		int fd1 = open(C->argv[1],O_RDONLY);//open source
		if(fd1<0)
			printf("Error opening source file. Does it exist?\n");
		else
		{
			int fd2 = open(C->argv[2],O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG);
			//opens or creates target
			if(fd2<0)
				printf("Error opening target file\n");
			else
			{
				int bytes;	
				int status;
				char buf[5000];//we have to specify a size! Big enough?
				bytes = lseek(fd1,0,SEEK_END);//get end of file
				lseek(fd1,0,SEEK_SET);//return pointer to start
				status = read(fd1,&buf,bytes-1);//read the whole file
				if(status==0)
					printf("Error reading source file\n");
				else
				{
					status = write(fd2,&buf,bytes-1);
					if(status<0)
						printf("Error writing to target file\n");
					else
						printf("Copy done\n");
				}
				close(fd2);
			}
			close(fd1);
		}
	}
}

void myrm(command * C)
{
//Unlinks file passed as argument
	if(C->argv[1]==NULL)//if no file was given
		printf("You haven't specified a file\n");
	else
	{
		int status;
		status = unlink(C->argv[1]);//unlink
		if(status==-1)
		{
			printf("File failed to delete\n");
		}
		else
			printf("File probably deleted\n");//haha, because I unlinked
	}	
}

void mycat(command * C)
{
//Print content of file passed as argument
	if(C->argv[1]==NULL) //if there is no argument
		printf("You haven't specified a file\n");
	else
	{
		int fd;
		fd = open(C->argv[1],O_RDONLY);
		if(fd<0)
			printf("Error opening file. Does it exist?\n");
		else
		{
			char* mapPos;
			int bytes;
			int i;
			bytes = lseek(fd, 0, SEEK_END);//get end of file to map all of it
			mapPos = mmap(0, bytes, PROT_READ, MAP_SHARED, fd, 0); //map all from start
			for (i=0; i<bytes;i++)//print all bytes one by one until end of file
			{
				printf("%c",mapPos[i]);
			}
			printf("\n");//add jump line at end
			munmap(mapPos, bytes); //delete map before finishing
		}
	}
}

void myls(command * C, char inid[])
{
//Lists entries of directory specified
//If there's no directory specified, uses current
//If -l is entered, more info is displayed
	bool l;//if there's -l or not
	char *path=inid;//dir path, stats at current
	char filePath[1024]; //for -l, get full path + file
	DIR *pDir;
	struct stat buf;
	struct dirent *pDirent;//structure of dir
	if(C->argv[1]!=NULL)//are there args?
	{
		if(strcmp(C->argv[1],"-l")==0)//first arg is ls
		{
			l = true;
			if(C->argv[2]==NULL)//no dir given
				pDir = opendir(".");
			else//dir given
			{
				path = C->argv[2];
				pDir = opendir(C->argv[2]);
			}
		}
		else //first arg is dir
		{
			pDir = opendir(C->argv[1]);
			path = C->argv[1];
			if(C->argv[2]==NULL)//no ls given
				l = false;
			else
			{
				if(strcmp(C->argv[2],"-l")==0)//is -l?
					l = true;
				else
					l = false;
			}
		}
	}
	else //no argument given (current directory, no ls)
	{
			l = false;
			pDir = opendir(".");//opens current directory
	}
	//after deciding if -l and opening dir, display
	if(l)//if -l is there
	{
		if (pDir == NULL)
			printf("Cannot open directory\n");
		else
		{
			while((pDirent = readdir(pDir)) != NULL)
			{
				sprintf(filePath, "%s/%s",path,pDirent->d_name);
				//printf("%s\n",filePath);
				if(stat(filePath,&buf) ==-1)//get stat structure of file
					perror("Error: ");
				else
				{
					printf("%s ", pDirent->d_name);//print name
					printf((S_ISDIR(buf.st_mode)) ? "d" : "-");//is dir?
					printf((buf.st_mode & S_IRUSR) ? "r" : "-");//owner reading
					printf((buf.st_mode & S_IWUSR) ? "w" : "-");//owner writing
					printf((buf.st_mode & S_IXUSR) ? "x" : "-");//owner execute
					printf((buf.st_mode & S_IRGRP) ? "r" : "-");//group reading
					printf((buf.st_mode & S_IWGRP) ? "w" : "-");//group writing
					printf((buf.st_mode & S_IXGRP) ? "x" : "-");//group execute
					printf((buf.st_mode & S_IROTH) ? "r" : "-");//others reading
					printf((buf.st_mode & S_IWOTH) ? "w" : "-");//others writing
					printf((buf.st_mode & S_IXOTH) ? "x" : "-");//others execute
					printf(" %d",buf.st_uid);//UID of owner
					printf(" %d",buf.st_gid);//GID of owner
					printf(" %ld",buf.st_size);//size <- check
					printf(" %s",ctime(&buf.st_mtim.tv_sec));//modification date
				}
				
			}
		}
		
	}
	else//only display names
	{
		if (pDir == NULL)
			printf("Cannot open directory\n");
		else
		{
			while((pDirent = readdir(pDir)) != NULL)
			{
				printf("%s ", pDirent->d_name);
			}
		printf("\n");
		}
	}
	closedir(pDir);//close directory whenever finished
}

