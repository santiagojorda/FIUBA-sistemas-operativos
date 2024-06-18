#ifndef DEFS_H
#define DEFS_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#ifndef SHELL_NO_COLORS
// color scape strings
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RED "\x1b[31m"
#define COLOR_RESET "\x1b[0m"
#else
#define COLOR_BLUE ""
#define COLOR_RED ""
#define COLOR_RESET ""
#endif

#define END_STRING '\0'
#define END_LINE '\n'
#define SPACE ' '

#define BUFLEN 1024
#define PRMTLEN 1024
#define MAXARGS 20
#define ARGSIZE 1024
#define FNAMESIZE 1024

// command representation after parsed
#define EXEC 1
#define BACK 2
#define REDIR 3
#define PIPE 4

// fd numbers for pipes
#define READ 0
#define WRITE 1

#define EXIT_SHELL 1

#endif  // DEFS_H
