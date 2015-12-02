/*
    parser.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "parser.h"

// Array containing the possible error messages (linked to the
// definitions made in parser.h):

const char * const err_messages[] =
{
    "All OK",                   //                          0
    "End of file",              // ERROR_EOF               -1
    "Wrong parameters",         // ERROR_BAD_PARAMETERS    -2
    "Not enough free memory"    // ERROR_NOT_ENOUGH_MEM    -3
};

// Declaration of functions private to this module (they can't
// be called from other modules):

static int read_line (char ** pline, FILE * stream);

/*  --------------------------------------------------------------
    init_command ()

    "Reset" a structure in order to pass it later to
    read_command().
*/

void init_command (command * C)
{
    if (C)
        memset (C, 0, sizeof(command));
}

/*  --------------------------------------------------------------
    read_command ()

    Read and parse a command taken from a file or from the
    keyboard, separating the different arguments, applying escape
    characters and quotation marks etc.

    Parameters:

        C           Structure where to store the parsed command

        stream      File or stream from where to read the command
                    (if it is NULL, the standard input will be
                    used)

    Result:

        If everything is OK, read_command() will return 0. It
        will return a negative value if an error occurs. In
        that case, the error message can be obtained changing
        the sign of the returned value and using it as an index
        in the array err_messages.

    Before using this function you must call init_command().
    Otherwise, read_command() will return the error code
    ERROR_BAD_PARAMETERS.

    After calling read_command(), once finished working with
    the parsed command, you must call free_command() in order
    to free the dynamic memory reserved by read_command().
*/

int read_command (command * C, FILE * stream)
{
    int n;
    char ** argv;       // Arguments vector
    int argc, argr;     // Size used/reserved of argv
    char * p;           // Current position in source buffer
    char * q;           // Current position in destination buffer
    char * piece;       // Current "word" on destination buffer
    char quotmark;      // '\'' or '\"' if quotation marks
    char escape;        // '\\' if escape sequence
    char redirection;   // 1=="<..."  2==">..."  3=="2>..."

    if (C == NULL || C->raw_command != NULL ||
                     C->pieces_buffer != NULL ||
                     C->argv != NULL)
        return ERROR_BAD_PARAMETERS;

    n = read_line (&C->raw_command, stream);

    if (n <= 0)     // If error or empty command,
        return n;   // done

    // Skip initial whitespace

    for (p=C->raw_command; *p && strchr(" \t\n",*p); p++)
    {}

    if (*p == '\0')    // If nothing remians, there is no command
        return 0;      // and no arguments

    // Allocate the buffer of command pieces (it won't be
    // initialized here because the command will be copied
    // on the fly)

    C->pieces_buffer = (char*) malloc ((1+strlen(p))*
                                       sizeof(char));

    if (C->pieces_buffer == NULL)
        return ERROR_NOT_ENOUGH_MEM;

    // Allocate array of pointers to arguments (just a few, by
    // now; the size will be duplicated later as many times as
    // it is required)

    argr = 4;
    C->argc = argc = 0;
    C->argv = argv = (char**) malloc (argr*sizeof(char*));

    if (argv == NULL)
        return ERROR_NOT_ENOUGH_MEM;

    // Traverse the buffer character by character, parsing the
    // different parts of the command.

    // The command will be carefully copied from C->raw_command
    // to C->pieces_buffer with operations like:  *q++ = *p++;

    // But in addition, escape sequences, quotation marks,
    // redirections, and & ---background execution--- will be
    // interpreted and applied.

    // Note that C->pieces_buffer will contain more than one
    // end-of-string character '\0', because it will host several
    // strings ---every argument and redirection---. The elements
    // of the array argv and the pointers of the redirections
    // will point to the inside of this buffer.

    C->input = C->output = C->output_err = NULL;
    quotmark = escape = redirection = C->background = 0;

    p = C->raw_command;
    piece = q = C->pieces_buffer;

    for (;;)
    {
        if (!escape && quotmark!='\'' && *p=='\\')
        {
            escape = *p++;        // Escape character...
            continue;             // "escape" the next char.
        }

        if (!escape && !quotmark && (*p=='\'' || *p=='\"'))
        {
            quotmark = *p++;      // Open quotation mark...
            continue;             // Raise the quotmark flag
        }

        if (!escape && quotmark && *p==quotmark)
        {
            quotmark = 0;         // Closing current quot. mark..
            p ++;                 // Put quotmark flag down
            continue;
        }

        if (*p && (escape || quotmark ||
                   !strchr(" \t\n<>&",*p)))
        {
            escape = 0;           // Normal char. (or escaped
            *q++ = *p++;          // by \ or quotmark)...
            continue;             // copy and advance
        }
                           // Special character or whitespace...
        *q++ = '\0';       // put an end to current piece

        if (*p=='>' && strcmp(piece,"2")==0 && q==piece+2)
        {
            redirection = 3;   // If "2>" was read, don't
            piece = q;         // store it, and take next piece
            p ++;              // as destination of error output
        }

        if (*piece)                 // Except for previous case,
        {                           // store current piece
            switch (redirection)    // wherever it corresponds
            {
                case 1:  C->input = piece;        break;
                case 2:  C->output = piece;       break;
                case 3:  C->output_err = piece;   break;

                case 0:             // Ordinary argument -> argv

                    if (argc+1 == argr) // If required, allocate
                    {                   // more space in argv
                        argr *= 2;      // (double it each time)
                        argv = (char**)
                               realloc (C->argv=argv,
                                        argr*sizeof(char**));

                        if (argv == NULL)
                            return ERROR_NOT_ENOUGH_MEM;
                    }

                    argv[argc++] = piece;
            }

            redirection = 0;      // Put redirection flag down
        }

        piece = q;     // Next piece starts at current position

        switch (*p)    // If special character, raise flag
        {
            case '&':  C->background = 1;  p ++;  break;
            case '<':  redirection = 1;    p ++;  break;
            case '>':  redirection = 2;    p ++;
        }

        while (*p && strchr(" \t\n",*p))  // Skip whitespace
            p ++;

        if (*p == '\0')        // End of source buffer...
        {                      // done
            C->argv = argv;
            C->argc = argc;
            argv[argc] = NULL;    // Mark the end of argv
            return 0;
        }
    }
}

/*  --------------------------------------------------------------
    free_command ()

    Free the dynamic memory of command read by read_command().

    Parameters:

        C           Structure where read_command() stored the
                    command.
*/

void free_command (command * C)
{
    if (C)
    {
        free (C->raw_command);
        free (C->pieces_buffer);
        free (C->argv);

        memset (C, 0, sizeof(command));
    }
}

/*  --------------------------------------------------------------
    read_line ()

    Read a line from a stream (the keyboard or a file) and store a
    copy in dynamic memory. The character '\n', which marks the
    end of the line, will _not_ be stored in the array.

    Parameters:

        pline       Must point to a char pointer which, in turn,
                    must point to NULL. It will be modified.

        stream      Data source (if NULL, stdin will be used).

    Result:

        The number of characters read (not including the
        end-of-line '\n') or an error code (lesser than zero)
        will be returned.

        In addition read_line() will write in *pline (the pointer
        passed by reference) the address of a new dynamic array
        containing the string read. The caller will be
        responsible of freeing this memory when it is not needed
        anymore.

        Note that the end of file and the lack of free memory
        will be considered as errors. Though, in both cases,
        some memory might have been reserved and might contain
        data. In these cases, the dynamic memory is returned
        as if no error had occurred. No data are lost in any
        case.
*/

static int read_line (char ** pline, FILE * stream)
{
    int size;    // Size of the array reserved
    int pos;     // Position inside the array
    char * p;    // Auxiliar pointer for calling realloc
    int c;       // Character read

    if (pline == NULL || *pline != NULL)
        return ERROR_BAD_PARAMETERS;

    if (stream == NULL)
        stream = stdin;

    size = 8;    // Initial size (later doubled as needed)
    pos = 0;
    *pline = p = (char*) malloc (size*sizeof(char));

    if (*pline == NULL)
        return ERROR_NOT_ENOUGH_MEM;

    for (;;)     // Inifinite loop
    {
        c = fgetc (stream);         // Read one character

        if (c=='\n' || c==EOF)      // Newline or
        {                           // end of file...
            p[pos] = '\0';
            return c=='\n' ? pos : ERROR_EOF; // ...finished
        }

        p[pos++] = c;               // Store the character

        if (pos == size)            // If required, enlarge the
        {                           // array... (double size)
            size *= 2;
            p = size<=0 ? NULL :
                (char*) realloc (*pline, size*sizeof(char));

            if (p == NULL)          // If it fails, *pline still
            {                       // points to previous array
                p = *pline;
                ungetc (c, stream);          // Refuse last char
                p[pos-1] = '\0';             // so that the end-
                return ERROR_NOT_ENOUGH_MEM; // of-string '\0'
            }                                // fits in the array

            *pline = p;   // Everything OK... point to new array
        }                 // and go on reading characters
    }
}


