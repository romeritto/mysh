#ifndef UTILS_H
#define	UTILS_H

#include <stddef.h> // size_t
#include <sys/types.h> // mode_t

/*
 * Note: The behaviour of mysh_getline is different from getline(3).
 *
 * It reads bytes from fd until EOF or EOL and stores them in a buffer
 * of size *n. This buffer is returned in **buf with trailing '\0'. The length
 * of the line is return in *n.
 * If more than *n - 1 bytes are read, **buf contains first *n - 1 bytes of
 * input and *n stores the length of the line.
 *
 * The return value is 1 on success, 0 if line ended with EOF and -1 on error
 * with errno set.
 */
int mysh_getline(char **buf, size_t *n, int fd);

void *safe_malloc(size_t n);
int safe_open(const char *pathname, int flags, mode_t mode);
void safe_sigint_block();
void safe_sigint_unblock();
void safe_sigint_setaction(void (*action)(int));

#endif // UTILS_H
