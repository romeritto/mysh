%{
#include "cmd.h"
#include "utils.h"

#include <stdio.h>
//#include <stdlib.h>
#include <sys/queue.h>
#include <assert.h>
#include <string.h> // strdup
#include <err.h>

// Bison needs to know about flex 
extern int yylex();

extern void yyerror(char *);

//extern int yydebug;
//yydebug = 1;

static void word_list_append(word_list_t * wl, word_t word) {
	word_en_t * word_en = (word_en_t *) safe_malloc(sizeof(word_en_t));
	word_en->value = word;
	STAILQ_INSERT_TAIL(wl, word_en, entries);
}

static redir_list_t * redir_list_append(redir_list_t * rl, int type_flag, word_t fname) {
    redir_t * redir = (redir_t *) safe_malloc(sizeof(redir_t));
    redir->type_flag = type_flag;
    redir->fname = fname;

    redir_en_t * redir_en = (redir_en_t *) safe_malloc(sizeof(redir_en_t));
	redir_en->value = redir;
    
	STAILQ_INSERT_TAIL(rl, redir_en, entries);
    return rl;
}

static redir_list_t * redir_list_create() {
    redir_list_t * rl = 
        (redir_list_t *) safe_malloc(sizeof(redir_list_t)); 
    STAILQ_INIT(rl);
    return rl;
}

static command_t * new_command(operator_t op, command_t *l, command_t *r, simple_command_t *scmd) {
    command_t * cmd = (command_t *) safe_malloc(sizeof(command_t));
    cmd->up = NULL;
    cmd->cmd1 = l;
    cmd->cmd2 = r;
    cmd->op = op;
    cmd->scmd = scmd;
    
    if (scmd != NULL) {
        assert(l == NULL && r == NULL && op == OP_NONE);
        scmd->up = cmd;
    } else {
        assert(l != NULL && r != NULL && op != OP_NONE);
        l->up = r->up = cmd;
    }
    return cmd;
}

static void process_redir_list(redir_list_t * rl, simple_command_t * scmd) {
    redir_en_t *redir_en;
    STAILQ_FOREACH(redir_en, rl, entries) {
        // Duplicates the fname string to ease the deallocation
        switch(redir_en->value->type_flag) {
            case IO_REDIRECT_IN:
                scmd->io_flags |= IO_REDIRECT_IN;
                scmd->redir_in = strdup(redir_en->value->fname);
                break;
            case IO_REDIRECT_OUT:
                // Remove append flag 
                scmd->io_flags &= ~IO_REDIRECT_APPEND_OUT;
                scmd->io_flags |= IO_REDIRECT_OUT;
                scmd->redir_out = strdup(redir_en->value->fname);
                break;
            case IO_REDIRECT_APPEND_OUT:
                // Remove out flag
                scmd->io_flags &= ~IO_REDIRECT_OUT;
                scmd->io_flags |= IO_REDIRECT_APPEND_OUT;
                scmd->redir_out = strdup(redir_en->value->fname);
                break;
           default:
                err(10, "Internal: unknown io_flag"); 
        } 
    }

    // Deallocate the list
    while (!STAILQ_EMPTY(rl)) {
        redir_en = STAILQ_FIRST(rl);
        STAILQ_REMOVE_HEAD(rl, entries);
        free(redir_en->value->fname);
        free(redir_en->value);
        free(redir_en);
    }
    free(rl);
}

static void print_word_list(word_list_t *wl) {
    word_en_t *word_en;
    STAILQ_FOREACH(word_en, wl, entries) {
        printf("\t%s\n", word_en->value);
    }
}
static void print_command_tree(command_t *cmd) {
    if (cmd->scmd != NULL) {
        printf("Simple cmd:\n");
        print_word_list(cmd->scmd->params);
        printf("\tRedir:\n");
        printf("\t\tflags %d\n", cmd->scmd->io_flags);
        printf("\t\tIN %s\n", cmd->scmd->redir_in);
        printf("\t\tOUT %s\n", cmd->scmd->redir_out);
        return;
    }
    print_command_tree(cmd->cmd1);
    print_command_tree(cmd->cmd2);
    switch(cmd->op) {
        case OP_PIPE: printf("|\n"); break;
        case OP_SEQUENTIAL: printf(";\n"); break;
        case OP_NONE: printf("We should error out\n");
    }
}
%}

%union {
	word_t word_un;
	word_list_t * word_list_un;
    redir_t redir_un;
	redir_list_t * redir_list_un;
	command_t * command_un;
	simple_command_t * simple_command_un;
}

%token END_OF_FILE END_OF_LINE
%token REDIRECT_IN REDIRECT_OUT REDIRECT_APPEND_OUT
%token <word_un> WORD

%left SEQUENTIAL
%left PIPE

%type <word_list_un> word_list
%type <redir_list_un> redirection
%type <redir_list_un> redirection_list
%type <command_un> command
/* %type redirect */
%type <simple_command_un> simple_command
%type <command_un> command_tree

%start command_root

%%

command_root: 
            command_tree {
                print_command_tree($1);
                YYACCEPT;
            }

command_tree:
            command END_OF_LINE { $$ = $1; }
            | command END_OF_FILE { $$ = $1; }
            | command SEQUENTIAL END_OF_LINE { $$ = $1; }
            | command SEQUENTIAL END_OF_FILE { $$ = $1; }
            | END_OF_LINE { $$ = NULL; }
            | END_OF_FILE { $$ = NULL; }

command:
            command PIPE command {
                $$ = new_command(OP_PIPE, $1, $3, NULL);
            }
            | command SEQUENTIAL command {
                $$ = new_command(OP_SEQUENTIAL, $1, $3, NULL);
            }
            | redirection_list simple_command redirection_list {
                process_redir_list($1, $2);
                process_redir_list($3, $2);
                $$ = new_command(OP_NONE, NULL, NULL, $2);
            }

redirection_list:
            /* empty */ { $$ = redir_list_create(); }
            | redirection_list redirection {
				STAILQ_CONCAT($1, $2);
                $$ = $1;
            }

redirection:
            REDIRECT_IN WORD {
				$$ = redir_list_create();
                redir_list_append($$, IO_REDIRECT_IN, $2);
            }
            | REDIRECT_OUT WORD {
				$$ = redir_list_create();
                redir_list_append($$, IO_REDIRECT_OUT, $2);
            }
            | REDIRECT_APPEND_OUT WORD {
				$$ = redir_list_create();
                redir_list_append($$, IO_REDIRECT_APPEND_OUT, $2);
            }

simple_command:
			word_list {
                simple_command_t * sc = 
                    (simple_command_t *) safe_malloc(sizeof(simple_command_t));
                sc->params = $1;
                sc->redir_in = sc->redir_out = NULL;
                sc->io_flags = IO_REGULAR;
                sc->up = NULL;
                $$ = sc;
			}

word_list:
            word_list WORD {
				word_list_append($1, $2);
                $$ = $1;
            }
            | WORD {
				word_list_t * wl = 
					(word_list_t *) safe_malloc(sizeof(word_list_t)); 
				STAILQ_INIT(wl);
				word_list_append(wl, $1);
				$$ = wl;
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

// Parkplatz
//word_en_t *word_en;
//STAILQ_FOREACH(word_en, $1, entries) {
//    printf("Read1: %s\n", word_en->value);
//}
//
//while (!STAILQ_EMPTY($1)) {
//    word_en = STAILQ_FIRST($1);
//    STAILQ_REMOVE_HEAD($1, entries);
//    free(word_en->value);
//    free(word_en);
//}
//free($1);
