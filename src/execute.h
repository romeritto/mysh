#ifndef EXECUTE_H
#define EXECUTE_H

#include "cmd.h"

void execute(seq_list_t *rootp);

/* Flag indicating whether mysh should exit (e.g. after exit command) */
extern int should_exit;

#endif // EXECUTE_H

