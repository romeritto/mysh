#ifndef __CMD_H
#define __CMD_H

#include <sys/queue.h>  // STAILQ
#include <stddef.h> 	// NULL
#include <stdlib.h>		// malloc

typedef char * word_t;

/* Single word entry in STAILQ linked-list */
struct word_en_s {
    word_t value;
    STAILQ_ENTRY(word_en_s) entries;
};
typedef struct word_en_s word_en_t;
typedef STAILQ_HEAD(word_list_s, word_en_s) word_list_t;

/* io_flags
 * IO_REDIRECT_OUT and IO_REDIRECT_APPEND_OUT are exclusive
 */
#define IO_REGULAR              0x00
#define IO_REDIRECT_IN          0x01
#define IO_REDIRECT_OUT         0x02
#define IO_REDIRECT_APPEND_OUT  0x04

/* Redirection info for parsing */
typedef struct {
    // One of io_flags
    int type_flag;
    word_t fname;
} redir_t;

struct redir_en_s {
    redir_t *value;
    STAILQ_ENTRY(redir_en_s) entries;
};
typedef struct redir_en_s redir_en_t;
typedef STAILQ_HEAD(redir_list_s, redir_en_s) redir_list_t;

/* Operators */
typedef enum {
	OP_NONE,
	OP_SEQUENTIAL,
	OP_PIPE
} operator_t;

typedef struct {
	word_list_t * params;
    word_t redir_in;
    word_t redir_out;
    int io_flags;
	struct command_t *up;
} simple_command_t;

typedef struct command_t {
	struct command_t *up;
	struct command_t *cmd1;
	struct command_t *cmd2;
	operator_t op;
	simple_command_t *scmd;
} command_t;

#endif // __CMD_H
