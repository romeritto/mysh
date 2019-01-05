#include <stdio.h>
#include <sys/queue.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "cmd.h"
#include "utils.h"

// Returns NULL terminated array of strings
char **
argq_to_array(arg_list_t * argqp) {
	size_t arg_count = 0;
	arg_en_t *arg_en;
	STAILQ_FOREACH(arg_en, argqp, entries) {
		++arg_count;
	}

	char ** arg_arr = 
		(char **) safe_malloc((arg_count + 1) * sizeof(char *));
	size_t i = 0;
	STAILQ_FOREACH(arg_en, argqp, entries) {	
		arg_arr[i++] = arg_en->value;	
	}
	arg_arr[i] = NULL;
	return (arg_arr);
}

void execute(seq_list_t *rootp) {
//  	if (rootp->scmd != NULL) {
// 		char **argv = argq_to_array(rootp->scmd->params);
// 		int pid;
// 		switch (pid = fork()) {
// 		case -1:
// 			perror("-mysh");
// 			// TODO: Deallocate shit and tune ret value
// 			exit(1);
// 		case 0:
// 			execvp(argv[0], argv);
// 			// Unreachable by default
// 			fprintf(stderr, "-mysh: %s: ", strerror(errno));
// 			// TODO: Return 127?
// 			break;
// 		default:
// 			// save pid and return;
// 			break;
// 		}
// 		// printf("\tRedir:\n");
// 		// printf("\t\tflags %d\n", rootp->scmd->io_flags);
// 		// printf("\t\tIN %s\n", rootp->scmd->redir_in);
// 		// printf("\t\tOUT %s\n", rootp->scmd->redir_out);
// 		return;
// 	}
// 	execute(rootp->cmd1);
// 	execute(rootp->cmd2);
// 	switch(rootp->op) {
// 		case OP_PIPE: printf("|\n"); break;
// 		case OP_SEQUENTIAL: printf(";\n"); break;
// 		case OP_NONE: printf("We should error out\n");
// 	}
}
