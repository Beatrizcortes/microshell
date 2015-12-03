/*
	internal_commands.h
*/

#ifndef _INTERNAL_COMMANDS_H_
#define _INTERNAL_COMMANDS_H_

#include <stdio.h>
#include "parser.h"

//extern char inid[256];//initial directory will be stored here globaly

void execute_internal_command(command * C, char inid[]);
void mypwd();
void mymkdir(command * C);
void myrmdir(command * C);
void mycd(command * C, char inid[]);
void mycp(command * C);
void myrm(command * C);
void mycat(command * C);
void myls(command * C);
#endif // _INTERNAL_COMMANDS_H_
