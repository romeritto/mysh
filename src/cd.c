#define _GNU_SOURCE // get_current_dir_name

#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int
cd(int argc, char **argv)
{
	char *newdir = NULL;
	
	/* Process arguments */	
	switch (argc) {
	case 1:
		newdir = getenv("HOME");
		if (newdir == NULL) {
			warnx("cd: HOME environment var not defined");
			return (1);
		}
		break;
	case 2:
		if (strcmp(argv[1], "-") == 0) {
			newdir = getenv("OLDPWD");
			if (newdir == NULL) {
				warnx(
				     "cd: OLDPWD environment var not defined");
				return (1);
			}
		} else {
			newdir = argv[1];
		}
		break;
	default:
		warnx("cd: too many arguments");
		return (1);
	}
	
	/* Change directory */
	if (chdir(newdir) == -1) {
		warn("cd: chdir");
		return (1);
	} 

	/* Gets full-path of the current dir. */
	newdir = get_current_dir_name();
	
	/* Update PWD and OLDPWD */
	if (setenv("OLDPWD", getenv("PWD"), /* overwrite = */ 1) == -1) {
		warn("cd: setenv");
		free(newdir);
		return (1);
	}

	if (setenv("PWD", newdir, /* overwrite = */ 1) == -1) {
		warn("cd: setenv");
		free(newdir);
		return (1);
	}
	
	free(newdir);
	return (0);
}
