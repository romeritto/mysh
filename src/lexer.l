/* Scanner for mysh */

%option noyywrap 

%{
#include "cmd.h"
#include "parser.h"

#include <string.h>

extern void yyerror(char *);
%}

%x COMMENT

WHITESPACE  		[ \r\t\f]
NON_SPECIAL_CHAR    [^ \r\t\f<>|;#\\'`"\]\[&\n]

%%

">"      return REDIRECT_OUT; 

">>"     return REDIRECT_APPEND_OUT;

"<"      return REDIRECT_IN; 

"|"      return PIPE;

";"      return SEQUENTIAL;

"#"                         BEGIN(COMMENT);

<COMMENT>[^\n]+             /* eat everything until EOL/EOF */

<COMMENT>\n                 { 
                                BEGIN(INITIAL);
								return END_OF_LINE;
                                /* TODO: increment newline */
                            }

\n                         return END_OF_LINE;

{NON_SPECIAL_CHAR}+         {
                                yylval.word_un = strdup(yytext);
                                return WORD;
                            }

{WHITESPACE}+               /* do nothing */

<<EOF>>                     return END_OF_FILE;

.                           yyerror("unknown char");
%%
