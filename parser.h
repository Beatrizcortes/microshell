/*
    parser.h
*/

#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>

// Possible error codes (linked to the array err_messages,
// whose value is established in parser.c):

#define ERROR_EOF               -1  // == EOF
#define ERROR_BAD_PARAMETERS    -2
#define ERROR_NOT_ENOUGH_MEM    -3

// Array containing error messages:

extern const char * const err_messages[];

// Structure that will hold a parsed command:

typedef struct
{
    char * raw_command;    // Full command as it was read
    char * pieces_buffer;  // Command pieced in arguments etc.

    // NOTE: Either redirections (input, output and output_err)
    //       and arguments (argv[0], argv[1], etc.) point to the
    //       inside of the block pieces_buffer instead of
    //       pointing to independent blocks

    char * input;          // Redirection of the input
    char * output;         // Redirection of the output
    char * output_err;     // Redir. of errors output

    int argc;              // Number (count) of arguments
    char ** argv;          // Arguments vector

    char background;       // 1: execute in the background ('&')
}                          // 0: no
command;

// Function that initialises a command (this function must be
// called before read_command()):

void init_command (command * C);

// Function that reads and parses a command:

int                               // 0 if OK; negative if error
    read_command
                 (command * C,      // Result
                  FILE * stream);   // Source of the data

// Function that frees the resources used by a command:

void free_command (command * C);

#endif // _PARSER_H_

