    int num_tokens = 0, num_line = 1;

DIGIT   [0-9]
LETTER  [a-zA-Z]
ESCAPE_CHAR [\\'"nt]


%%
"+"|"-"|"*"|"/"                 { num_tokens++; }
"=="|"!="|"<="|">="|"<"|">"     { num_tokens++; }
"="                             { num_tokens++; }
"&"                             { num_tokens++; }   
";"|"("|")"|"{"|"}"             { num_tokens++; }
"if"                            { num_tokens++; }
"else"                          { num_tokens++; }
"while"                         { num_tokens++; }
"char"                          { num_tokens++; }
"int"                           { num_tokens++; }
"float"                         { num_tokens++; }
"return"                        { num_tokens++; }
"void"                          { num_tokens++; }
\'\\{ESCAPE_CHAR}\'             { num_tokens++; }
\'{LETTER}\'                    { num_tokens++; }
\"(\\.|[^"\\])*\"               { num_tokens++; }
({LETTER}|_)({LETTER}|{DIGIT}|_)*     { num_tokens++; }
{DIGIT}+({LETTER}|_)+({LETTER}|{DIGIT}|_)*     { printf("[ERROR] Invalid Identifier on line %d\n", num_line); 
                                  exit(0); }
{DIGIT}+                        { num_tokens++; }
{DIGIT}+\.{DIGIT}+              { num_tokens++; }
[ \t]                           /* skip whitespace */
[\n\r]                          { num_line++; }
.                               { printf("[ERROR] Unknown Character [%c] on line %d\n", yytext[0], num_line); 
                                  exit(0); }


%%
int main (argc, argv) 
int argc;
char **argv;
{
    // Set yyin stream to given arg filename or stdin
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
    } else {
        yyin = stdin;
    }

    yylex();
    printf("%d\n", num_tokens);

    return 0;
}
