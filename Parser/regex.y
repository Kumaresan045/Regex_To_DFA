%{
  #include "NFA.h"
  extern int yylex(void); // Flex lexer function
  int yyerror(char*);
%}

%union
{
  class NFA* nfa;
}

%type<nfa> regex maybe_or maybe_concat maybe_kleene maybe_terminal program


%%
program: regex
  {
      $$ = $1->makeDFA();
      $$ = $$->minimalDFA();
      cout << $$->getCode() << '\n';
  }
;

regex: maybe_or
      { 
        $$ = $1;
      } 
;

maybe_or: maybe_or '|' maybe_concat
      { 
        $$ = $1->makeUnion($1,$3);
      } 
    
    |  maybe_concat
      {
        $$ = $1;
      }
;

maybe_concat: maybe_concat maybe_kleene 
      { 
        $$ = $1->makeConcat($1,$2);
      } 
    
    |  maybe_kleene
      {
        $$ = $1;
      }
;

maybe_kleene: maybe_terminal '*' 
      { 
        $$ = $1->makeKleeneStar();
      } 
    
    |  maybe_terminal
      {
        $$ = $1;
      }
;

maybe_terminal: 'a' 
      { 
        $$ = new NFA("a"); 
      } 
    
    | 'b'
      {
        $$ = new NFA("b");
      }
    |  '(' regex ')' 
      {
        $$ = $2;
        $$->regex.str("");
        $$->regex.clear();
        
        $$->regex << '(' << $$->getRegex() << ')';
      }

;

%%

int yyerror(char *s) {
  fprintf(stderr, "%s\n", s);
  return 1;
}

int main(void) {
  
  yyparse();
  return 0;
}
