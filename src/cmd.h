#ifndef __CMD_H
#define __CMD_H

typedef enum {
	OP_NONE,
	OP_SEQUENTIAL,
	OP_PIPE
} operator_t;

typedef struct {
	char *params;
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
