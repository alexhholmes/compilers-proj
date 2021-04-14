%{
  int tokens = 0;
%}

IDENTIFIER ([a-zA-Z]|_)([a-zA-Z]|_|[0-9])*
INTEGER [0-9]+
FLOATP [0-9]+\.[0-9]+
CHARACTER \'[a-zA-Z]\'
STRING \"(\\.|[^"\\])*\"
ARITHMETIC [+-/*]
COMPARISON ==|!=|<=|<|>=|>
ASSIGNMENT =
POINTER &
SEPARATOR [(){};]
WHITESPACE \t| |\n


%%
{IDENTIFIER} {printf("IDENTIFIER "); tokens++;}
{INTEGER} {printf("INTEGER "); tokens++;}
{FLOATP} {printf("FLOAT "); tokens++;}
{CHARACTER} {printf("CHARACTER "); tokens++;}
{STRING} {printf("STRING "); tokens++;}
{ARITHMETIC} {printf("ARITHMETIC "); tokens++;}
{COMPARISON} {printf("COMPARISON "); tokens++;}
{ASSIGNMENT} {printf("ASSIGNMENT "); tokens++;}
{POINTER} {printf("POINTER "); tokens++;}
{SEPARATOR} {printf("SEPARATOR"); tokens++;}

%%

 int main(int argc, char **argv)
 {
     ++argv, --argc;  /* Skip over program name */
     if (argc > 0)
             yyin = fopen( argv[0], "r" );
     else
             yyin = stdin;

     yylex();
     printf("%d\n", tokens);
 }
