#include <sys/queue.h>
#include <stdlib.h>

#include "cmd.h"
#include "utils.h"

redir_list_t *
create_redir_list() {
	redir_list_t * rl = (redir_list_t *) safe_malloc(sizeof(redir_list_t));
	STAILQ_INIT(rl);
	return rl;
}

redir_list_t *
append_redir_list(
	redir_list_t * rl,
	redir_type type,
	char * fname)
{
	redir_t * redir = (redir_t *) safe_malloc(sizeof(redir_t));
	redir->type = type;
	redir->fname = fname;

	redir_en_t * redir_en = (redir_en_t *) safe_malloc(sizeof(redir_en_t));
        redir_en->value = redir;

	STAILQ_INSERT_TAIL(rl, redir_en, entries);
	return rl;
}

piped_list_t *append_piped_list(piped_list_t *pl, command_t *cmd)
{
	piped_en_t * piped_en = (piped_en_t *) safe_malloc(sizeof(piped_en_t));
        piped_en->value = cmd;

	STAILQ_INSERT_TAIL(pl, piped_en, entries);
	return pl;
}

seq_list_t *append_seq_list(seq_list_t *sl, piped_list_t *pl)
{
	seq_en_t * seq_en = (seq_en_t *) safe_malloc(sizeof(seq_en_t));
        seq_en->value = pl;

	STAILQ_INSERT_TAIL(sl, seq_en, entries);
	return sl;
}

arg_list_t *append_arg_list(arg_list_t *al, char *arg)
{
	arg_en_t * arg_en = (arg_en_t *) safe_malloc(sizeof(arg_en_t));
        arg_en->value = arg;

	STAILQ_INSERT_TAIL(al, arg_en, entries);
	return al;
}

void free_arg_list(arg_list_t *al)
{
	if (al == NULL) return;
	arg_en_t *entryp;
	while (!STAILQ_EMPTY(al)) {
		entryp = STAILQ_FIRST(al);
		STAILQ_REMOVE_HEAD(al, entries);
		free(entryp->value);
		free(entryp);
	}
	free(al);
}

void free_redir_list(redir_list_t *rl)
{
	if (rl == NULL) return;
	redir_en_t *entryp;
	while (!STAILQ_EMPTY(rl)) {
		entryp = STAILQ_FIRST(rl);
		STAILQ_REMOVE_HEAD(rl, entries);
		free_redir(entryp->value);
		free(entryp);
	}
	free(rl);
}

void free_command(command_t *cmdp)
{
	if (cmdp == NULL) return;
	free_arg_list(cmdp->arg_list);
	free_redir_list(cmdp->redir_list);
	free(cmdp);
}

void free_piped_list(piped_list_t *pl)
{
	if (pl == NULL) return;
	piped_en_t *entryp;
	while (!STAILQ_EMPTY(pl)) {
		entryp = STAILQ_FIRST(pl);
		STAILQ_REMOVE_HEAD(pl, entries);
		free_command(entryp->value);
		free(entryp);
	}
	free(pl);
}

void free_seq_list(seq_list_t *sl)
{
	if (sl == NULL) return;
	seq_en_t *entryp;
	while (!STAILQ_EMPTY(sl)) {
		entryp = STAILQ_FIRST(sl);
		STAILQ_REMOVE_HEAD(sl, entries);
		free_piped_list(entryp->value);
		free(entryp);
	}
	free(sl);
}

void free_redir(redir_t *redirp)
{
	if (redirp == NULL) return;
	free(redirp->fname);
	free(redirp);
}

