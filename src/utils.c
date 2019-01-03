#include "utils.h"

#include <stdlib.h>	// malloc
#include <stdio.h>  // stderr
#include <errno.h>  // errno
#include <string.h> // strerror
#include <stddef.h> // size_t

void* safe_malloc(size_t n)
{
    void* p = malloc(n);
    if (!p)
    {
		fprintf(stderr, "malloc failed (%s)\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return p;
}
