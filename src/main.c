#include <err.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>

#include "execute.h"
#include "utils.h"

#define	PROMPT_PREFIX "mysh:"
#define PROMPT_SUFFIX "$ "
/* ARG_MAX was not defined on lab machine in limits.h */
#define MAX_LINE_LEN 100000

typedef enum { MODE_FILE, MODE_CODE, MODE_INTERACTIVE, MODE_UNDEF } Mode;

static int serve_interactive();
static int serve_code(char *code);
static int serve_file(char *fname);
static char *get_prompt();
static void interactive_sigint_handler(int sig);

void
usage(char *argv0)
{
	errx(1, "usage: %s [-c code | file]", argv0);
}

int
main(int argc, char **argv)
{
	int opt;
	char *argv0 = basename(argv[0]);
	char *code = NULL;
	char *fname = NULL;
	Mode mode = MODE_UNDEF;

	/* Suppress getopt() error messages. */
	opterr = 0;

	while ((opt = getopt(argc, argv, "c:")) != -1) {
		switch (opt) {
		case 'c':
			/*
			 * optarg should be copied since it might be
			 * overwritten by another option or freed by getopt()
			 */
			if ((code = strdup(optarg)) == NULL)
				err(1, "cannot alloc memory for -c optarg");
			mode = MODE_CODE;
			break;
		case '?':
			fprintf(stderr, "unknown option: '%c'\n", optopt);
			usage(argv0);
			break;
		}
	}

	/* optind is the 1st non-option argument. */
	argv += optind;
	argc -= optind;

	if (argc > 1) {
		fprintf(stderr, "too many filenames\n");
		usage(argv0);
	} else if (argc == 1) {
		if (mode == MODE_UNDEF) {
			fname = strdup(argv[0]);
			mode = MODE_FILE;
		} else {
			fprintf(stderr, "both -c code and filename"
					"specified\n");
			usage(argv0);
		}
	}

	if (mode == MODE_UNDEF)
		mode = MODE_INTERACTIVE;

	switch (mode) {
	case MODE_INTERACTIVE:
		serve_interactive();
		break;
	case MODE_CODE:
		serve_code(code);
		free(code);
		break;
	case MODE_FILE:
		serve_file(fname);
		free(fname);
		break;
	default:
		fprintf(stderr, "internal err: unknown mode");
		assert(0);
	}
	

	return (return_val);
}

static int
serve_interactive() {
    safe_sigint_block();
	using_history();
	while (!is_exit_terminated) {
		char *prompt = get_prompt();
		safe_sigint_setaction(interactive_sigint_handler);
        safe_sigint_unblock();
		char *line = readline(prompt);
        safe_sigint_block();

		free(prompt);
		if (line == NULL) break;
		if (strcmp(line, "") != 0) add_history(line);

        safe_sigint_setaction(execution_sigint_handler);
		execute_line(line, /* line_num */ 1);
		free(line);
	}

	/* Free memory used by history. */
 	HISTORY_STATE *myhist = history_get_history_state();
 	HIST_ENTRY **mylist = history_list();
 	for (int i = 0; i < myhist->length; ++i)
 		free_history_entry(mylist[i]);

 	free(myhist);
 	free(mylist);

	return (0);
}

static int
serve_code(char *code) {
    safe_sigint_block();
	safe_sigint_setaction(execution_sigint_handler);
	execute_line(code, /* line_num */ 1);
	return (0);
}

static int
serve_file(char *fname) {
    safe_sigint_block();
	safe_sigint_setaction(execution_sigint_handler);
	int fd = safe_open(fname, O_RDONLY, /* unused */ 0);
	int line_num = 1;
	while (!is_exit_terminated && !is_sigint_terminated) {
		char *line;
		size_t n = MAX_LINE_LEN;
		int ret_getline = mysh_getline(&line, &n, fd);
		if (ret_getline < 0) err(1, "read");
		if (n > MAX_LINE_LEN) errx(1, "%d: line too long", line_num);
		int ret_execute = execute_line(line, line_num++);
		free(line);

		/* EOF */
		if (ret_getline == 0) break;
		/* Parse error */
		if (ret_execute != 0) break;
	}
	return (0);
}

/* Allocates a prompt with the format 'PROMPT_PREFIX + pwd + PROMPT_SUFFIX'. */
static char *
get_prompt()
{
	char *pwd = getenv("PWD");
	size_t prompt_len =
		strlen(PROMPT_PREFIX) + strlen(pwd) + strlen(PROMPT_SUFFIX); 

	char *prompt = (char *) safe_malloc((prompt_len + 1) * sizeof(char));
	strcpy(prompt, PROMPT_PREFIX);
	strcat(prompt, pwd);
	strcat(prompt, PROMPT_SUFFIX);
	return (prompt);
}

static void
interactive_sigint_handler(int sig)
{
		/* Reset the readline() */
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
}
