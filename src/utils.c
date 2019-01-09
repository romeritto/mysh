#include <fcntl.h>
#include <sys/types.h> // mode_t
#include <stdlib.h> // malloc
#include <stdio.h>  // stderr
#include <errno.h>  // errno
#include <err.h> // err
#include <string.h> // strerror
#include <stddef.h> // size_t
#include <unistd.h>
#include <signal.h>

#include "utils.h"

static void safe_sigemptyset(sigset_t *set);
static void safe_sigaddset(sigset_t *set, int signum);
static void safe_sigpending(sigset_t *set);
static void safe_sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
static int safe_sigismember(sigset_t *set, int signum);

void *
safe_malloc(size_t n)
{
    void* p = malloc(n);
    if (!p)
    {
		fprintf(stderr, "malloc failed (%s)\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return p;
}

int
safe_open(const char *pathname, int flags, mode_t mode) {
        int fd;
        if ((fd = open(pathname, flags, mode)) == -1)
                err(1, "%s", pathname);
        return fd;
}

int
mysh_getline(char **buf, size_t *n, int fd)
{
	char *buf_ = (char *) safe_malloc(*n * sizeof(char));
	char c;
	size_t i = 0;
	ssize_t status;
	while ((status = read(fd, &c, /* buf_size */ 1)) == 1) {
		if (c == '\n')
			break;
		else 
			if (i >= *n) { i++; continue; }
			else buf_[i++] = c;
	}
	if (i < *n)
		buf_[i] = '\0';
	*buf = buf_;
	*n = i;
	return status;
}

void safe_sigint_block() {
	sigset_t mask;
	safe_sigemptyset(&mask);
	safe_sigaddset(&mask, SIGINT);
	safe_sigprocmask(SIG_BLOCK, &mask, NULL);
}

void safe_sigint_unblock() {
	sigset_t mask;
	safe_sigemptyset(&mask);
	safe_sigaddset(&mask, SIGINT);
	safe_sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void safe_sigint_setaction(void (*action)(int))
{
	struct sigaction act = { 0 };
	act.sa_handler = action;
	/* Set restart flag so that waitpid restarts. */
	act.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &act, NULL) == -1) {
		perror("");
		exit(1);
	}
}

void
sigint_serve()
{
	/*
	 * I didn't find whether sigsuspend(2) calls handler multiple times
	 * if the signal is triggered multiple times. On my machine it is the
	 * case, but it might call the handler only once. That's why I used
	 * the do-while cycle.
	 */
	
	int is_sigint_pending;
	sigset_t pending;
	do {
		safe_sigemptyset(&pending);
		safe_sigpending(&pending);
		is_sigint_pending = safe_sigismember(&pending, SIGINT);
		if (is_sigint_pending) {
			/* Unblocks all signals. But we only block SIGINT... */
			sigset_t empty;
			safe_sigemptyset(&empty);
			sigsuspend(&empty);
		}
	} while (is_sigint_pending);
}

static void
safe_sigemptyset(sigset_t *set)
{
	/* sigemptyset does not set errno. */
	if (sigemptyset(set) == -1) {
		exit(1);
	}
}

static void
safe_sigaddset(sigset_t *set, int signum)
{
	/* sigaddset does not set errno. */
	if (sigaddset(set, signum) == -1) {
		exit(1);
	}
}

static void 
safe_sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
	if (sigprocmask(how, set, oldset) == -1) {
                perror("");
		exit(1);
	}
}

static void
safe_sigpending(sigset_t *set)
{
	if (sigpending(set) == -1) {
                perror("");
		exit(1);
	}
}

static int 
safe_sigismember(sigset_t *set, int signum)
{
	int ret;
	if ((ret = sigismember(set, signum)) == -1) {
                perror("");
		exit(1);
	}
	return ret;
}
