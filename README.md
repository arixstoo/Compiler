# Compiler


INSTRUCTIONS IN CMD


cd path... <br>
flex lexical.l <br>
bison -d syn.y <br>
gcc lex.yy.c syn.tab.c quadruples.c symbole.c -o my_compiler <br>
my_compiler < test1.txt <br>
