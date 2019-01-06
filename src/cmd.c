#include <sys/queue.h>

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
