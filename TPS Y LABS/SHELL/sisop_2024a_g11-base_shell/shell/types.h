#ifndef TYPES_H
#define TYPES_H

#include "defs.h"

/* This file contains definition types for commands representation */

/*
 * cmd
 * ---
 *
 *  Generic interface that represents a single command.
 *  All the other *cmd structs can be casted to it,
 *  and they don´t lose information (for example the 'type' field).
 *
 *  - type: { EXEC, REDIR, BACK, PIPE }
 *  - pid: the process id
 *  - scmd: a string representing the command before being parsed
 */
struct cmd {
	int type;
	pid_t pid;
	char scmd[BUFLEN];
};

/*
 * execcmd
 * -------
 *
 * It contains all the relevant information to execute a command.
 *
 *  - type: could be EXEC or REDIR
 *  - argc: arguments quantity after parsed
 *  - argv: array of strings representig the arguments
 *          of the form: { "binary/command", "arg0", "arg1", ... , (char*) NULL }
 *  - eargc: environ vars quantity after parsed
 *  - eargv: array of strings of the form: "KEY=VALUE"
 *          representing the environ vars
 *  - *_file: string that contains the name of the file to be redirected to
 *
 *  IMPORTANT
 *  ---------
 *      An execcmd struct can have EXEC or REDIR type depending on if the command
 *      to be executed has at least one redirection symbol (<, >, >>, >&)
 */
struct execcmd {
	int type;
	pid_t pid;
	char scmd[BUFLEN];
	int argc;
	int eargc;
	char *argv[MAXARGS];
	char *eargv[MAXARGS];
	char out_file[FNAMESIZE];
	char in_file[FNAMESIZE];
	char err_file[FNAMESIZE];
};

/*
 * pipecmd
 * --------
 *
 * It contains the same information as 'cmd' plus two fields representing the left
 * and right part of a command of the form: "command1 arg1 arg2 | command2 arg3"
 */
struct pipecmd {
	int type;
	pid_t pid;
	char scmd[BUFLEN];
	struct cmd *leftcmd;
	struct cmd *rightcmd;
};

/*
 * backcmd
 * -------
 *
 * It contains the same information as 'cmd' plus one more field containing the
 * command to be executed. Take a look to the parsing.c file for better understandig.
 *
 * Again, this extra field, can have type either EXEC or REDIR depending on
 * if the process to be executed in the background contains redirection symbols.
 */
struct backcmd {
	int type;
	pid_t pid;
	char scmd[BUFLEN];
	struct cmd *c;
};

#endif  // TYPES_H
