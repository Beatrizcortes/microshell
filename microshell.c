/*
    microshell.c
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"
#include "internal_commands.h"
void show_command (command * C);

int main ()
{
    command C;
    int r;
    bool cont = true;
    char inid[256];//get initial path
    getcwd(inid, 256);

    printf ("Welcome to the Micro Shell!\n");

    do              // Read commands and show them
    {
        init_command (&C);

        printf ("$ ");
        r = read_command (&C, stdin);

        if (r < 0)
            fprintf (stderr, "\nError %d: %s\n",
                             -r, err_messages[-r]);
        else
	{
	    if(C.argc!=0)
	    {
	    	if (strcmp(C.argv[0], "exit")==0)
	    	{
		    cont=false;
	    	}
	    	else
	    	{
	    	    execute_internal_command(&C,inid);//pass comand and initial path
		    //I could have made that path global but memory was unassigned
	    	}
	    }	
        free_command (&C);
	}
    }
    while (r==0 && cont);   // Repeat until error, exit or EOF

    return 0;
}

void show_command (command * C)
{
//used for debug but keeping it here just in case.
//Came with the template
    int i;

    printf ("\tRaw command: \"%s\"\n", C->raw_command);
    printf ("\tNumber of arguments: %d\n", C->argc);

    for (i=0; i<=C->argc; i++)
        if (C->argv[i] != NULL)
            printf ("\t\targv[%d]: \"%s\"\n", i, C->argv[i]);
        else
            printf ("\t\targv[%d]: NULL\n", i);

    if (C->input)
        printf ("\tInput: \"%s\"\n", C->input);

    if (C->output)
        printf ("\tOutput: \"%s\"\n", C->output);

    if (C->output_err)
        printf ("\tErr. output: \"%s\"\n", C->output_err);

    printf ("\tExecute in background: %s\n",
            C->background ? "Yes" : "No");
}


