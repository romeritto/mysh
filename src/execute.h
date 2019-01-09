#ifndef EXECUTE_H
#define EXECUTE_H

#include "cmd.h"

/* Executes line. Returns 0 on success and 1 on error. */ 
int execute_line(char *line, int line_num);
void yyerror(const char *msg);
void execution_sigint_handler(int sig);

/* Flag indicating exit was called. */
extern int is_exit_terminated;
/* Flag indicating SIGINT while execution of command was caught. */
extern int is_sigint_terminated;
/* Return value of the last command on line. */
extern int return_val;

#endif // EXECUTE_H

