#ifndef CMD_H
#define	CMD_H

#include <stddef.h> 	// size_t
#include <sys/queue.h>  // STAILQ

typedef enum { REDIR_IN, REDIR_OUT, REDIR_APPEND_OUT } redir_type;

/* Redirection info for parsing. */
typedef struct {
	redir_type type;
	char *fname;
} redir_t;

/* Single argument entry. */
struct arg_en_s {
	char *value;
	STAILQ_ENTRY(arg_en_s) entries;
};

/* Single redirection entry. */
struct redir_en_s {
	redir_t *value;
	STAILQ_ENTRY(redir_en_s) entries;
};

typedef struct arg_en_s arg_en_t;
typedef struct redir_en_s redir_en_t;
typedef STAILQ_HEAD(arg_list_s, arg_en_s) arg_list_t;
typedef STAILQ_HEAD(redir_list_s, redir_en_s) redir_list_t;

/* Simple command. */
typedef struct {
	arg_list_t *arg_list;
	redir_list_t *redir_list;
} command_t;

/* Single piped list entry. It's just a list of commands in REVERSED order. */
struct piped_en_s {
	command_t *value;
	STAILQ_ENTRY(piped_en_s) entries;
};

typedef struct piped_en_s piped_en_t;
typedef STAILQ_HEAD(piped_list_s, piped_en_s) piped_list_t;

/* Single sequential list entry. It's just a list of piped_lists... */
struct seq_en_s {
	piped_list_t *value;
	STAILQ_ENTRY(seq_en_s) entries;
};

typedef struct seq_en_s seq_en_t;
typedef STAILQ_HEAD(seq_list_s, seq_en_s) seq_list_t;

/*
 * Parses a single line.
 *      line            -- a line to parse
 *      line_num        -- number of the line (used for the error output)
 *      rootpp (return) -- root of the parsed structure
 *
 * Returns zero on success, non-zero value otherwise.
 */
int parse_line(char * line, int line_num, seq_list_t **rootpp);

redir_list_t *create_redir_list();
redir_list_t *append_redir_list(
	redir_list_t * rl,
	redir_type type,
	char * fname);
piped_list_t *append_piped_list(piped_list_t *pl, command_t *cmd);
seq_list_t *append_seq_list(seq_list_t *sl, piped_list_t *pl);
arg_list_t *append_arg_list(arg_list_t *al, char *arg);

void free_arg_list(arg_list_t *al);
void free_redir_list(redir_list_t *rl);
void free_command(command_t *cmdp);
void free_piped_list(piped_list_t *pl);
void free_seq_list(seq_list_t *sl);
void free_redir(redir_t *redirp);

#endif // CMD_H
