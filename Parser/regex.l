%{
	#include <stdlib.h>
	#include "regex.tab.h"
	void yyerror(char *);
	extern char mytext[];
%}

%%

[ab]		{ return *yytext;}

[()|*]		{ return *yytext; }

[ \n\t]	{ }

.	{yyerror("unknown character");}
%%

int yywrap(void) {
    return 1;
}



