# Compiler


INSTRUCTIONS IN CMD

flex lexical.l
bison -d syn.y
gcc lex.yy.c syn.tab.c quadruples.c symbole.c -o my_compiler
