%option	noyywrap nostdinit nounput noinput

%{
#include "cmd.h"
#include "parser.h"

#include <string.h>

extern void yyerror(char *);
%}

%x COMMENT

WHITESPACE		[ \r\t\f]
NON_SPECIAL_CHAR	[^ \r\t\f<>|;#\\'`"\]\[&\n]

%%

">"	return (REDIRECT_OUT);

">>"	return (REDIRECT_APPEND_OUT);

"<"	return (REDIRECT_IN);

"|"	return (PIPE);

";"	return (SEQUENTIAL);

"#"			BEGIN(COMMENT);

<COMMENT>[^\n]+		/* Eat everything until EOL/EOF. */

<COMMENT>\n		{
				BEGIN(INITIAL);
				return (END_OF_LINE);
			}

\n			return (END_OF_LINE);

{NON_SPECIAL_CHAR}+	{
				yylval.word_un = strdup(yytext);
				return (WORD);
			}

{WHITESPACE}+		/* Do nothing. */

<<EOF>>			return (END_OF_FILE);

.			yyerror("Unknown char");

%%

YY_BUFFER_STATE my_state;

void
init_lexer_with_line(const char * line)
{
	yylex_destroy();
	my_state = yy_scan_string(line);
	BEGIN(INITIAL);
}
