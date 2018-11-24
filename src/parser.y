%{
#include "cmd.h"

#include <stdio.h>
#include <stdlib.h>

// Bison needs to know about flex 
extern int yylex();

extern void yyerror(char *);

//extern int yydebug;
//yydebug = 1;
%}

%union {
	char * string_un;
	command_t * command_un;
	simple_command_t * simple_command_un;
}


%token END_OF_FILE END_OF_LINE
%token REDIRECT_IN
%token REDIRECT_OUT
%token REDIRECT_APPEND_OUT
%token <string_un> WORD

%left SEQUENTIAL
%left PIPE

%type <command_un> command
/* %type redirect */
%type <simple_command_un> simple_command

%start command_tree

%%

command_tree:
            command END_OF_LINE {
               printf("parsed root EOL\n");
               YYACCEPT;
            }
            | command END_OF_FILE {
                printf("parsed root EOF\n");
                YYACCEPT;
            }
            | command SEQUENTIAL END_OF_LINE {
                /* same handler as above */
                printf("end of sequential\n");
				YYACCEPT;
            }
            | command SEQUENTIAL END_OF_FILE {
                /* same handler as above */
                printf("end of sequential\n");
				YYACCEPT;
            }
            | END_OF_LINE {
                // root = null
                printf("root: blankline\n");
                YYACCEPT;
            }
            | END_OF_FILE {
                // root = null
                printf("root: blank\n");
                YYACCEPT;
            }

command:
            command PIPE command {
                printf("piped commands");
            }
            | command SEQUENTIAL command {
                printf("sequential commands");
            }
            | simple_command redirections {
                printf("found simple cmd\n");
            }

redirections:
            /* empty */ { printf("jooo\n"); }
            | redirections redirection {
                printf("another redirection\n");
            }

redirection:
            REDIRECT_IN WORD {
                printf("in redirection\n");
            }
            | REDIRECT_OUT WORD {
                printf("out redirection\n");
            }
            | REDIRECT_APPEND_OUT WORD {
                printf("append redirection\n");
            }

simple_command:
			word_list { 
				printf("command found\n");
			}

word_list:
            word_list WORD | WORD {
                printf("found word\n");
            }
%%

int main(void) {
	// Parse through the input:
	yyparse();
	return 0;
}

void yyerror(char *s) {
	printf("%s\n", s);
	// might as well halt now:
	exit(-1);
}
