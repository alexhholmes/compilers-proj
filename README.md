# csci4570-project-3
### Alex Holmes & Lana Kelly

## Compiling
`flex tinyc.l`
`bison -v -t -d tinylex.y`
`gcc ast.c lex.yy.c tinylex.tab.c symtab.c codegen.c -g -o parser`

## Running
`./parser < test.c`

## Compiling and Running Ouput
`gcc -m32 output.s -o output`
`./output`
