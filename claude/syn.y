%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quadruples.h"
#include "symbole.h"

extern int yylineno;
extern int column;
int yylex(void);
int yyerror(char *msg);

// Variables globales
char currentScope[20] = "GLOBAL";  // Pour gérer la portée
extern int numQuad;                  // Pour la gestion des quadruplets
char temp[20];                     // Pour stocker les variables temporaires
int nTemp = 1;                     // Compteur pour les variables temporaires
int jumpStack[100];                // Pile pour gérer les sauts
int jumpTop = 0;                   // Sommet de la pile de sauts

// Fonction pour générer des variables temporaires
char* newTemp() {
    sprintf(temp, "T%d", nTemp++);
    // Ajouter la variable temporaire à la table des symboles
    insererSymbole(temp, "FLOAT", "VARIABLE", currentScope, yylineno);
    return strdup(temp);
}

// Fonctions pour la gestion des sauts
void pushJump(int quad) {
    jumpStack[jumpTop++] = quad;
}

int popJump() {
    return jumpStack[--jumpTop];
}

%}

%union {
    int integer;
    float real;
    char character;
    char* string;
    struct {
        char* type;
        char* value;
        char* temp;
    } exp;
}

/* Tokens */
%token VAR_GLOBAL DECLARATION INSTRUCTION
%token <string> INTEGER FLOAT CHAR
%token <string> IDF
%token <integer> CONST_INT
%token <real> CONST_FLOAT
%token <character> CONST_CHAR
%token <string> CONST_STRING  //machi sûr menha

/* Mots clés */
%token CONST IF ELSE FOR READ WRITE

/* Opérateurs */
%token EQ NEQ LT GT LTE GTE
%token AND OR NOT

/* Priorités et associativité */
%left OR
%left AND
%left NOT
%left EQ NEQ LT GT LTE GTE
%left '+' '-'
%left '*' '/'
%right UNARY_MINUS

%type <exp> expression
%type <string> condition type comparison_op
%type <exp> constant
%type <string> for_condition  /* Ajout du type manquant */

%start program

%%

program
    : VAR_GLOBAL {
        strcpy(currentScope, "GLOBAL");
    } '{' declarations '}' 
      DECLARATION {
        strcpy(currentScope, "LOCAL");
      } '{' declarations '}' 
      INSTRUCTION '{' instructions '}'
      {
          printf("\nAnalyse syntaxique et sémantique terminée avec succès\n");
          afficherTS();
          afficherQuad();
      }
    ;

declarations
    : /* vide */
    | declarations declaration
    ;

declaration
    : type var_list ';' 
    | const_declaration
    | array_declaration
    ;

type
    : INTEGER { $$ = "INTEGER"; }
    | FLOAT { $$ = "FLOAT"; }
    | CHAR { $$ = "CHAR"; }
    ;

var_list
    : IDF {
        int idx = insererSymbole($1, "VARIABLE", currentScope, currentScope, yylineno);
        if (idx == -1) {
            yyerror("Erreur de déclaration");
        }
    }
    | var_list ',' IDF {
        int idx = insererSymbole($3, "VARIABLE", currentScope, currentScope, yylineno);
        if (idx == -1) {
            yyerror("Erreur de déclaration");
        }
    }
    ;

const_declaration
    : CONST type IDF '=' constant ';' {
        int idx = insererSymbole($3, "CONSTANT", $2, currentScope, yylineno);
        if (idx != -1) {
            // Initialiser la constante avec sa valeur
            setConstanteValeur(idx, $2, &($5.value));
        }
    }
    ;

array_declaration
    : type IDF '[' CONST_INT ']' ';' {
        if ($4 <= 0) {
            yyerror("La taille du tableau doit être positive");
        }
        int idx = insererSymbole($2, "TABLEAU", $1, currentScope, yylineno);
        if (idx != -1) {
            modifierTaille(idx, $4);
        }
    }
    ;

constant
    : CONST_INT {
        $$.type = "INTEGER";
        sprintf(temp, "%d", $1);
        $$.value = strdup(temp);
    }
    | CONST_FLOAT {
        $$.type = "FLOAT";
        sprintf(temp, "%f", $1);
        $$.value = strdup(temp);
    }
    | CONST_CHAR {
        $$.type = "CHAR";
        sprintf(temp, "'%c'", $1);
        $$.value = strdup(temp);
    }
    ;

instructions
    : /* vide */
    | instructions instruction
    ;

instruction
    : assignment
    | if_statement
    | for_statement
    | io_statement
    ;

assignment
    : IDF '=' expression ';' {
        int idx = rechercherSymbole($1);
        if (idx == -1) {
            yyerror("Variable non déclarée");
        } else {
            insererQuadComplet("=", $3.temp, "", $1);
            ts.table[idx].initialise = 1;
        }
    }
    ;

if_statement
    : IF '(' condition ')' {
        char quad_num[20];
        sprintf(quad_num, "%d", numQuad + 2);
        insererQuadComplet("BZ", $3, quad_num, "");
        pushJump(numQuad - 1);
    } '{' instructions '}' else_part
    ;

else_part
    : /* vide */ {
        int quad = popJump();
        char quad_num[20];
        sprintf(quad_num, "%d", numQuad);
        ajouterQuad(quad, 4, quad_num);
    }
    | ELSE {
        char quad_num[20];
        sprintf(quad_num, "%d", numQuad + 1);
        insererQuadComplet("BR", quad_num, "", "");
        int quad = popJump();
        sprintf(quad_num, "%d", numQuad + 1);
        ajouterQuad(quad, 4, quad_num);
    } '{' instructions '}'
    ;

for_statement
    : FOR '(' for_init ';' for_condition ';' for_step ')' {
        pushJump(numQuad - 2);
        char quad_num[20];
        sprintf(quad_num, "%d", numQuad + 2);
        insererQuadComplet("BZ", $5, quad_num, "");
    } '{' instructions '}' {
        char quad_num[20];
        int quad = popJump();
        sprintf(quad_num, "%d", quad);
        insererQuadComplet("BR", quad_num, "", "");
    }
    ;

for_init
    : IDF '=' expression {
        int idx = rechercherSymbole($1);
        if (idx == -1) {
            yyerror("Variable non déclarée");
        } else {
            insererQuadComplet("=", $3.temp, "", $1);
            ts.table[idx].initialise = 1;
        }
    }
    ;

for_step
    : IDF '=' expression {
        int idx = rechercherSymbole($1);
        if (idx == -1) {
            yyerror("Variable non déclarée");
        } else {
            insererQuadComplet("=", $3.temp, "", $1);
        }
    }
    ;

for_condition
    : condition
    ;

io_statement
    : READ '(' IDF ')' ';' {
        int idx = rechercherSymbole($3);
        if (idx == -1) {
            yyerror("Variable non déclarée");
        } else {
            insererQuadComplet("READ", "", "", $3);
            ts.table[idx].initialise = 1;
        }
    }
    | WRITE '(' write_args ')' ';'
    ;

write_args
    : expression {
        insererQuadComplet("WRITE", $1.temp, "", "");
    }
    | CONST_STRING {
        insererQuadComplet("WRITE", $1, "", "");
    }
    | write_args ',' expression {
        insererQuadComplet("WRITE", $3.temp, "", "");
    }
    | write_args ',' CONST_STRING {
        insererQuadComplet("WRITE", $3, "", "");
    }
    ;

expression
    : CONST_INT {
        $$.temp = newTemp();
        char val[20];
        sprintf(val, "%d", $1);
        insererQuadComplet("=", val, "", $$.temp);
        $$.type = "INTEGER";
    }
    | CONST_FLOAT {
        $$.temp = newTemp();
        char val[20];
        sprintf(val, "%f", $1);
        insererQuadComplet("=", val, "", $$.temp);
        $$.type = "FLOAT";
    }
    | CONST_CHAR {
        $$.temp = newTemp();
        char val[20];
        sprintf(val, "'%c'", $1);
        insererQuadComplet("=", val, "", $$.temp);
        $$.type = "CHAR";
    }
    | IDF {
        int idx = rechercherSymbole($1);
        if (idx == -1) {
            yyerror("Variable non déclarée");
            $$.temp = newTemp();
        } else if (!ts.table[idx].initialise) {
            yyerror("Variable non initialisée");
            $$.temp = newTemp();
        } else {
            $$.temp = strdup($1);
            $$.type = strdup(ts.table[idx].type);
        }
    }
    | expression '+' expression {
        $$.temp = newTemp();
        insererQuadComplet("+", $1.temp, $3.temp, $$.temp);
    }
    | expression '-' expression {
        $$.temp = newTemp();
        insererQuadComplet("-", $1.temp, $3.temp, $$.temp);
    }
    | expression '*' expression {
        $$.temp = newTemp();
        insererQuadComplet("*", $1.temp, $3.temp, $$.temp);
    }
    | expression '/' expression {
        $$.temp = newTemp();
        insererQuadComplet("/", $1.temp, $3.temp, $$.temp);
    }
    | '(' expression ')' {
        $$ = $2;
    }
    | '-' expression %prec UNARY_MINUS {
        $$.temp = newTemp();
        insererQuadComplet("*", $2.temp, "-1", $$.temp);
    }
    ;

condition
    : expression comparison_op expression {
        $$ = newTemp();
        insererQuadComplet($2, $1.temp, $3.temp, $$);
    }
    | condition AND condition {
        $$ = newTemp();
        insererQuadComplet("AND", $1, $3, $$);
    }
    | condition OR condition {
        $$ = newTemp();
        insererQuadComplet("OR", $1, $3, $$);
    }
    | NOT condition {
        $$ = newTemp();
        insererQuadComplet("NOT", $2, "", $$);
    }
    | '(' condition ')' {
        $$ = $2;
    }
    ;

comparison_op
    : EQ { $$ = strdup("=="); }
    | NEQ { $$ = strdup("!="); }
    | LT { $$ = strdup("<"); }
    | GT { $$ = strdup(">"); }
    | LTE { $$ = strdup("<="); }
    | GTE { $$ = strdup(">="); }
    ;

%%

int yyerror(char *msg) {
    printf("Erreur syntaxique ligne %d, colonne %d : %s\n", yylineno, column, msg);
    return 0;
}

int main() {
    initialiserTS();
    printf("Début de l'analyse...\n");
    yyparse();
    return 0;
}