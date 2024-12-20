%{
#include "syn.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symbole.h"

// Structure for lexical statistics
struct {
    char* idfs[1000];
    int idf_count;
    char* keywords[1000];
    int keyword_count;
    char* separators[1000];
    int separator_count;
} lexical_stats = {0};

int column = 1;
extern YYSTYPE yylval;
extern char* currentScope;

void add_to_array(char* arr[], int* count, const char* str) {
    for(int i = 0; i < *count; i++) {
        if(arr[i] && strcmp(arr[i], str) == 0) {
            return;
        }
    }
    arr[*count] = strdup(str);
    (*count)++;
}

void print_lexical_stats() {
    printf("\n=== Analyse Lexicale Statistics ===\n");
    
    printf("\nIdentificateurs (%d):\n", lexical_stats.idf_count);
    for(int i = 0; i < lexical_stats.idf_count; i++) {
        printf("%s ", lexical_stats.idfs[i]);
    }
    
    printf("\n\nMots Prédéfinis (%d):\n", lexical_stats.keyword_count);
    for(int i = 0; i < lexical_stats.keyword_count; i++) {
        printf("%s ", lexical_stats.keywords[i]);
    }
    
    printf("\n\nSéparateurs (%d):\n", lexical_stats.separator_count);
    for(int i = 0; i < lexical_stats.separator_count; i++) {
        printf("%s ", lexical_stats.separators[i]);
    }
    printf("\n");
}

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

"VAR_GLOBAL"   { column += yyleng; add_to_array(lexical_stats.keywords, &lexical_stats.keyword_count, yytext); return VAR_GLOBAL; }
"DECLARATION"  { column += yyleng; add_to_array(lexical_stats.keywords, &lexical_stats.keyword_count, yytext); return DECLARATION; }
"INSTRUCTION"  { column += yyleng; add_to_array(lexical_stats.keywords, &lexical_stats.keyword_count, yytext); return INSTRUCTION; }

"INTEGER"      { 
    column += yyleng; 
    add_to_array(lexical_stats.keywords, &lexical_stats.keyword_count, yytext);
    yylval.string = strdup(yytext);
    return INTEGER; 
}
"FLOAT"        { 
    column += yyleng;
    add_to_array(lexical_stats.keywords, &lexical_stats.keyword_count, yytext); 
    yylval.string = strdup(yytext);
    return FLOAT; 
}
"CHAR"         { 
    column += yyleng;
    add_to_array(lexical_stats.keywords, &lexical_stats.keyword_count, yytext); 
    yylval.string = strdup(yytext);
    return CHAR; 
}

"CONST"        { column += yyleng; add_to_array(lexical_stats.keywords, &lexical_stats.keyword_count, yytext); return CONST; }
"IF"           { column += yyleng; add_to_array(lexical_stats.keywords, &lexical_stats.keyword_count, yytext); return IF; }
"ELSE"         { column += yyleng; add_to_array(lexical_stats.keywords, &lexical_stats.keyword_count, yytext); return ELSE; }
"FOR"          { column += yyleng; add_to_array(lexical_stats.keywords, &lexical_stats.keyword_count, yytext); return FOR; }
"READ"         { column += yyleng; add_to_array(lexical_stats.keywords, &lexical_stats.keyword_count, yytext); return READ; }
"WRITE"        { column += yyleng; add_to_array(lexical_stats.keywords, &lexical_stats.keyword_count, yytext); return WRITE; }

"=="           { column += yyleng; add_to_array(lexical_stats.separators, &lexical_stats.separator_count, yytext); return EQ; }
"!="           { column += yyleng; add_to_array(lexical_stats.separators, &lexical_stats.separator_count, yytext); return NEQ; }
"<"            { column += yyleng; add_to_array(lexical_stats.separators, &lexical_stats.separator_count, yytext); return LT; }
">"            { column += yyleng; add_to_array(lexical_stats.separators, &lexical_stats.separator_count, yytext); return GT; }
"<="           { column += yyleng; add_to_array(lexical_stats.separators, &lexical_stats.separator_count, yytext); return LTE; }
">="           { column += yyleng; add_to_array(lexical_stats.separators, &lexical_stats.separator_count, yytext); return GTE; }
"&&"           { column += yyleng; add_to_array(lexical_stats.separators, &lexical_stats.separator_count, yytext); return AND; }
"||"           { column += yyleng; add_to_array(lexical_stats.separators, &lexical_stats.separator_count, yytext); return OR; }
"!"            { column += yyleng; add_to_array(lexical_stats.separators, &lexical_stats.separator_count, yytext); return NOT; }

{IDF}           {
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
        add_to_array(lexical_stats.idfs, &lexical_stats.idf_count, yytext);
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

{STRING} {
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

[;,\(\)\[\]\{\}] { 
    column += yyleng; 
    add_to_array(lexical_stats.separators, &lexical_stats.separator_count, yytext);
    return yytext[0]; 
}

.       {
    column += yyleng;
    add_to_array(lexical_stats.separators, &lexical_stats.separator_count, yytext);
    return yytext[0];   
}

%%