#include "freecmd.h"

// frees the memory allocated
// for the tree structure command
void
free_command(struct cmd *cmd)
{
	int i;
	struct pipecmd *p;
	struct execcmd *e;
	struct backcmd *b;

	if (cmd->type == PIPE) {
		p = (struct pipecmd *) cmd;

		free_command(p->leftcmd);
		free_command(p->rightcmd);

		free(p);
		return;
	}

	if (cmd->type == BACK) {
		b = (struct backcmd *) cmd;

		free_command(b->c);
		free(b);
		return;
	}

	e = (struct execcmd *) cmd;

	for (i = 0; i < e->argc; i++)
		free(e->argv[i]);

	for (i = 0; i < e->eargc; i++)
		free(e->eargv[i]);

	free(e);
}
