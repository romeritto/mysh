#include <assert.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h> // open types O_RDONLY, ...
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/queue.h>
#include <sys/stat.h> // permissions
#include <unistd.h>

#include "execute.h"
#include "cd.h"
#include "cmd.h"
#include "utils.h"

#define	FD_UNUSED -1
#define	NULL_PID -1
#define	RETVAL_UNKNOWN_CMD 127
#define	RETVAL_SIGNAL 128
#define	RETVAL_SYNTAX_ERR 254

typedef int *pid_arr_t;

/* Globals. */
int is_sigint_terminated = 0;
int is_exit_terminated = 0;
int return_val = 0;

/* Holds PIDs of processes spawned by current command. */
static pid_arr_t pid_arr = NULL;

static void execute_seq(seq_list_t *rootp);
static void execute_piped(piped_list_t *pl);
static int execute_command(command_t *cmdp, int fdin, int fdout, int fdX);

static void handle_redirections(redir_list_t *rl);
static char **arg_list_to_argv_view(arg_list_t * argqp, int *argc);
static void safe_pipe(int pd[]);
static void replace_fd(int oldfd, int newfd);

void
execution_sigint_handler(int sig)
{
	assert(sig == SIGINT);
	assert(pid_arr != NULL);
	pid_arr_t pid_arr_it = pid_arr;
	while (*pid_arr_it != NULL_PID) {
		kill(*pid_arr_it, sig);
		++pid_arr_it;
	}
	fprintf(stderr, "Killed by signal %d.\n", sig);
	is_sigint_terminated = 1;
}

int
execute_line(char *line, int line_num)
{
	seq_list_t *rootp;
	if (parse_line(line, line_num, &rootp) == 0) {
		execute_seq(rootp);
		free_seq_list(rootp);
		return (0);
	} else {
		return_val = RETVAL_SYNTAX_ERR;
		return (1);
	}
}

static void
execute_seq(seq_list_t *rootp)
{
	if (rootp == NULL)
		return;
	is_sigint_terminated = 0;
	seq_en_t *seq_en;
	STAILQ_FOREACH(seq_en, rootp, entries) {
		/* Fills pid_arr. */
		execute_piped(seq_en->value);
		pid_arr_t pid_arr_it = pid_arr;
		safe_sigint_unblock();
		while (*pid_arr_it != NULL_PID) {
			int stat_val = 0;
			waitpid(*pid_arr_it, &stat_val, 0);
			if (WIFEXITED(stat_val)) {
				return_val = WEXITSTATUS(stat_val);
			} else if (WIFSIGNALED(stat_val)) {
				return_val =
					RETVAL_SIGNAL + WTERMSIG(stat_val);
			} else {
				return_val = -1;
			}
			++pid_arr_it;
		}
		safe_sigint_block();
		free(pid_arr);
		pid_arr = NULL;
		if (is_exit_terminated || is_sigint_terminated)
			return;
	}
}

static void
execute_piped(piped_list_t *pl)
{
	int cmd_count = 0;
	piped_en_t *piped_en;
	STAILQ_FOREACH(piped_en, pl, entries) {
		++cmd_count;
	}

	/* NULL_PID terminated array of pids. */
	pid_arr = (pid_arr_t) safe_malloc((cmd_count + 1) * sizeof (int));
	pid_arr[0] = NULL_PID;

	/* Treat single cd/exit command specially. */
	if (cmd_count == 1) {
		command_t *cmdp = STAILQ_FIRST(pl)->value;
		char *cmd_name = STAILQ_FIRST(cmdp->arg_list)->value;
		if (strcmp("cd", cmd_name) == 0) {
			int argc;
			char **argv =
				arg_list_to_argv_view(cmdp->arg_list, &argc);
			/* cd ignores redirections */
			return_val = cd(argc, argv);
			free(argv);
			return;
		} else if (strcmp("exit", cmd_name) == 0) {
			is_exit_terminated = 1;
			return;
		}
	}

	int cmd_idx = 0;
	int is_first_command = 1;
	int pd[2];	/* intermediate pipes */
	STAILQ_FOREACH(piped_en, pl, entries) {
		int is_last_command =
			(STAILQ_NEXT(piped_en, entries) == NULL);
		int fdin, fdout,	/* I/O fd used for piping */
		    fdX;		/* redundant FD */

		fdin = (is_first_command) ? FD_UNUSED : pd[0];
		if (is_last_command) {
			fdout = FD_UNUSED;
			fdX = FD_UNUSED;
		} else {
			safe_pipe(pd);
			fdout = pd[1];
			fdX = pd[0];
		}

		pid_arr[cmd_idx++] = execute_command(
				piped_en->value, fdin, fdout, fdX);
		pid_arr[cmd_idx] = NULL_PID;

		if (fdin != FD_UNUSED)	close(fdin);
		if (fdout != FD_UNUSED) close(fdout);
		is_first_command = 0;
	}
}

/*
 * Executes command.
 *      cmdp            -- command to execute
 *      fdin            -- input file descriptor
 *      fdout           -- output file descriptor
 *      fdX             -- redundant file descriptor that should be closed in
 *                         the forked process
 *
 * Returns pid of the created process.
 */
static int
execute_command(command_t *cmdp, int fdin, int fdout, int fdX)
{
	int argc;
	char **argv = arg_list_to_argv_view(cmdp->arg_list, &argc);
	int pid;
	switch (pid = fork()) {
	case -1:
		perror("-mysh: fork");
		exit(1);
	case 0:
		safe_sigint_setaction(SIG_DFL);
		safe_sigint_unblock();

		if (fdin != FD_UNUSED)	replace_fd(fdin, 0);
		if (fdout != FD_UNUSED) replace_fd(fdout, 1);
		if (fdX != FD_UNUSED) close(fdX);

		/* Ignore cd and exit in piped commands */
		if (strcmp("cd", argv[0]) == 0 || strcmp("exit", argv[0]) == 0)
			exit(0);

		handle_redirections(cmdp->redir_list);
		execvp(argv[0], argv);

		/* Probably unknown command. */
		fprintf(
			stderr,
			"-mysh: %s: %s\n",
			argv[0], strerror(errno));
		exit(RETVAL_UNKNOWN_CMD);
	default:
		break;
	}
	free(argv);
	return (pid);
}

static void
handle_redirections(redir_list_t *rl)
{
	int default_permissions =
		S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	redir_en_t *redir_en;
	STAILQ_FOREACH(redir_en, rl, entries) {
		redir_t *redirp = redir_en->value;
		int fd;
		switch (redirp->type) {
		case REDIR_IN:
			fd = safe_open(
				redirp->fname, O_RDONLY, /* unused = */ 0);
			replace_fd(fd, /* newfd = */ 0);
			break;
		case REDIR_OUT:
			fd = safe_open(
				redirp->fname,
				O_WRONLY | O_CREAT | O_TRUNC,
				default_permissions);
			replace_fd(fd, /* newfd = */ 1);
			break;
		case REDIR_APPEND_OUT:
			fd = safe_open(
				redirp->fname,
				O_WRONLY | O_CREAT | O_APPEND,
				default_permissions);
			replace_fd(fd, /* newfd = */ 1);
			break;
		}
	}
}

static void
safe_pipe(int pd[])
{
	if (pipe(pd) != 0) {
		perror("-mysh: pipe");
		exit(1);
	}
}

/* Replaces oldfd with newfd and closes oldfd. */
static void
replace_fd(int oldfd, int newfd)
{
	if (dup2(oldfd, newfd) == -1) {
		perror("-mysh: dup2");
		exit(1);
	}
	close(oldfd);
}

/*
 * Returns NULL terminated array of string pointers from arg_list. After the
 * call argc will contain the number of arguments in arg_list.
 * The function doesn't duplicate the strings, hence it just creates a view.
 */
static char **
arg_list_to_argv_view(arg_list_t *argqp, int *argc)
{
	*argc = 0;
	arg_en_t *arg_en;
	STAILQ_FOREACH(arg_en, argqp, entries) {
		++(*argc);
	}

	char **argv = (char **) safe_malloc((*argc + 1) * sizeof (char *));
	size_t i = 0;
	STAILQ_FOREACH(arg_en, argqp, entries) {
		argv[i++] = arg_en->value;
	}
	argv[i] = NULL;
	return (argv);
}
