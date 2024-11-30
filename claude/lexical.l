%{
#include "syn.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symbole.h"


int column = 1;
extern YYSTYPE yylval;
extern char* currentScope;

int yywrap(void) {
    return 1;
}

%}


%option yylineno
%option nounput
%option noinput

lettre [A-Z]
chiffre [0-9]
IDF {lettre}({chiffre}|[a-z])*
INTEGER [0-9]+|[\(][+-][0-9]+[\)]
FLOAT [0-9]+\.[0-9]+|[\(][+-][0-9]+\.[0-9]+[\)]
CHAR \'[^\']*\'
STRING \"[^\"]*\"
COMMENT %%.*

%%

"VAR_GLOBAL"   { column += yyleng; return VAR_GLOBAL; }
"DECLARATION"  { column += yyleng; return DECLARATION; }
"INSTRUCTION"  { column += yyleng; return INSTRUCTION; }

"INTEGER"      { 
    column += yyleng; 
    yylval.string = strdup(yytext);
    return INTEGER; 
}
"FLOAT"        { 
    column += yyleng; 
    yylval.string = strdup(yytext);
    return FLOAT; 
}
"CHAR"         { 
    column += yyleng; 
    yylval.string = strdup(yytext);
    return CHAR; 
}

"CONST"        { column += yyleng; return CONST; }
"IF"           { column += yyleng; return IF; }
"ELSE"         { column += yyleng; return ELSE; }
"FOR"          { column += yyleng; return FOR; }
"READ"         { column += yyleng; return READ; }
"WRITE"        { column += yyleng; return WRITE; }

"=="           { column += yyleng; return EQ; }
"!="           { column += yyleng; return NEQ; }
"<"            { column += yyleng; return LT; }
">"            { column += yyleng; return GT; }
"<="           { column += yyleng; return LTE; }
">="           { column += yyleng; return GTE; }
"&&"           { column += yyleng; return AND; }
"||"           { column += yyleng; return OR; }
"!"            { column += yyleng; return NOT; }

{IDF} {
    column += yyleng;
    if(strlen(yytext) > 8) {
        printf("Erreur lexicale ligne %d, colonne %d: identificateur '%s' dépasse 8 caractères\n", 
               yylineno, column, yytext);
    }
    else if(yytext[0] < 'A' || yytext[0] > 'Z') {
        printf("Erreur lexicale ligne %d, colonne %d: identificateur '%s' doit commencer par une majuscule\n", 
               yylineno, column, yytext);
    }
    else {
        yylval.string = strdup(yytext);
        return IDF;
    }
}

{INTEGER} {
    column += yyleng;
    int val;
    if(yytext[0] == '(') {
        // Gestion des entiers signés entre parenthèses
        char *ptr = yytext + 1;
        val = atoi(ptr);
    } else {
        val = atoi(yytext);
    }
    if(val < -32768 || val > 32767) {
        printf("Erreur lexicale ligne %d, colonne %d: entier '%s' hors limites [-32768,32767]\n", 
               yylineno, column, yytext);
    } else {
        yylval.integer = val;
        return CONST_INT;
    }
}

{FLOAT} {
    column += yyleng;
    yylval.real = atof(yytext);
    return CONST_FLOAT;
}

{CHAR} {
    column += yyleng;
    yylval.character = yytext[1];
    return CONST_CHAR;
}

{STRING} {                      //machi sûr menha
    column += yyleng;
    yylval.string = strdup(yytext);
    return CONST_STRING;
}

{COMMENT} { 
    column += yyleng; 
    /* Ignore les commentaires */ 
}

[ \t]   { column += yyleng; }
\n      { column = 1; }

.       {                       //machi sûr menha
    column += yyleng;
    return yytext[0];   
}

%%