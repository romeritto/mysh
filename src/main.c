#include <err.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "cmd.h"
#include "execute.h"

#define	PROMPT "mysh:$ "

typedef enum { MODE_FILE, MODE_CODE, MODE_INTERACTIVE, MODE_UNDEF } Mode;

int serve_interactive();
int serve_code(char *code);
int serve_file(char *fname);

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

	printf("code: %s\nfname: %s\nmod: %d\n", code, fname, mode);
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
	return (0);
}

int
serve_interactive() {
	using_history();
	char *line = NULL;
	while ((line = readline(PROMPT)) != NULL) {
		seq_list_t *rootp;
		if (parse_line(line, 1, &rootp) == 0) execute(rootp);
		if (strcmp(line, "") != 0) add_history(line);
		if (should_exit) break;
	}
	return (0);
}

int
serve_code(char *code) {
	seq_list_t *root;
	parse_line(code, 1, &root);
	return (0);
}

int
serve_file(char *unused) {
	return (0);
}
