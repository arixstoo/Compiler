%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quadruples.h"
#include "symbole.h"
#define MAX_LOOPS 100

extern int yylineno;
extern int column;
int yylex(void);
int yyerror(char *msg);
char currentScope[20] = "";
char* currentType = NULL;
extern int numQuad;
char temp[20];
int nTemp = 1;
int jumpStack[100];
int jumpTop = 0;
char* loopVariables[MAX_LOOPS];
int currentLoop = -1;

// Structure pour gérer les niveaux d'imbrication des boucles
typedef struct {
    int start;      // Start of the loop
    int condition;  // Position of condition evaluation
    int step;       // Position of step execution
    char* stepValue; 
    int end;        // End of the loop
} BranchInfo;


typedef struct {
    int condition;
    char* stepValue;
    char* loopVar;
} LoopInfo;

typedef struct {
    LoopInfo* loops;
    int capacity;
    int top;
    int initialized;
} LoopStack;


static LoopStack loopStack = {NULL, 0, -1, 0};


#define MAX_BRANCH_DEPTH 50
BranchInfo branchStack[MAX_BRANCH_DEPTH];
int branchTop = 0;

// fonctions pour la gestion des branchements
void pushBranch(int start, int condition, int end) {
    if (branchTop < MAX_BRANCH_DEPTH) {
        branchStack[branchTop].start = start;
        branchStack[branchTop].condition = condition;
        branchStack[branchTop].end = end;
        branchTop++;
    }
}

BranchInfo* topBranch() {
    return branchTop > 0 ? &branchStack[branchTop - 1] : NULL;
}
void print_lexical_stats();

void popBranch() {
    if (branchTop > 0) branchTop--;
}

// générer des variables temporaires
char* newTemp() {
    sprintf(temp, "T%d", nTemp++);
    insererSymbole(temp, "FLOAT", "VARIABLE", currentScope, yylineno);
    return strdup(temp);
}

// Fonction pour mettre à jour les adresses de saut
void updateJumpAddress(int quadIndex, int targetAddress) {
    char temp[20];
    sprintf(temp, "%d", targetAddress);
    ajouterQuad(quadIndex, 4, temp);
}


static void ensureLoopStackInitialized() {
    if (!loopStack.initialized) {
        loopStack.loops = (LoopInfo*)malloc(sizeof(LoopInfo) * 10); // Start with 10 slots
        if (loopStack.loops == NULL) {
            yyerror("Failed to initialize loop stack");
            exit(1);
        }
        loopStack.capacity = 10;
        loopStack.top = -1;
        loopStack.initialized = 1;
    }
}

// Resize the stack 
static void resizeLoopStack() {
    int newCapacity = loopStack.capacity * 2;
    LoopInfo* newLoops = (LoopInfo*)realloc(loopStack.loops, sizeof(LoopInfo) * newCapacity);
    if (newLoops == NULL) {
        yyerror("Memory allocation failed for loop stack");
        exit(1);
    }
    loopStack.loops = newLoops;
    loopStack.capacity = newCapacity;
}

// Push loop info to stack
static void pushLoop(char* loopVar) {
    ensureLoopStackInitialized();
    if (loopStack.top + 1 >= loopStack.capacity) {
        resizeLoopStack();
    }
    loopStack.top++;
    loopStack.loops[loopStack.top].loopVar = strdup(loopVar);
    loopStack.loops[loopStack.top].stepValue = NULL;
    loopStack.loops[loopStack.top].condition = -1;
}

// Get current loop info
static LoopInfo* getCurrentLoop() {
    ensureLoopStackInitialized();
    if (loopStack.top < 0) {
        yyerror("No active loops");
        exit(1);
    }
    return &loopStack.loops[loopStack.top];
}

// Pop loop info from stack
static void popLoop() {
    if (!loopStack.initialized || loopStack.top < 0) {
        yyerror("Stack underflow in loop stack");
        exit(1);
    }
    free(loopStack.loops[loopStack.top].loopVar);
    free(loopStack.loops[loopStack.top].stepValue);
    loopStack.top--;
}


%}

%union {
    int integer;
    float real;
    char character;
    char* string;
    struct {
        char* type;
        char* value;  // Ensure this is a char*
        char* temp;
        int array_size;
    } exp;
}

/* Tokens */
%token VAR_GLOBAL DECLARATION INSTRUCTION
%token <string> INTEGER FLOAT CHAR
%token <string> IDF
%token <integer> CONST_INT
%token <real> CONST_FLOAT
%token <character> CONST_CHAR
%token <string> CONST_STRING

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

%type <exp> expression array_elem
%type <string> condition type comparison_op
%type <exp> constant

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
          print_lexical_stats();
          afficherTS();
          afficherQuad();
      };

declarations
    : /* vide */
    | declarations declaration
    ;

declaration
    : type var_list ';' 
    | const_declaration
    ;

type
    : INTEGER { currentType = "INTEGER"; $$ = "INTEGER"; }
    | FLOAT { currentType = "FLOAT"; $$ = "FLOAT"; }
    | CHAR { currentType = "CHAR"; $$ = "CHAR"; }
    ;

var_list
    : var_decl {
    }
    | var_list ',' var_decl {
    }
    ;

var_decl
    : IDF {
        int idx = insererSymbole($1, currentType, "VARIABLE", currentScope, yylineno);
        if (idx == -1) {
            yyerror("Erreur de déclaration");
        }
    }
    | IDF '[' CONST_INT ']' {
        if ($3 <= 0) {
            yyerror("La taille du tableau doit être positive");
        }
        
        // Insérer le tableau de base
        int baseIdx = insererSymbole($1, currentType, "TABLEAU", currentScope, yylineno);
        
        if (baseIdx != -1) {
            // Créer des entrées individuelles pour chaque élément du tableau
            for (int i = 0; i < $3; i++) {
                int elemIdx = insererElementTableau($1, i, currentType, currentScope, yylineno);
                if (elemIdx == -1) {
                    yyerror("Erreur lors de la création des éléments du tableau");
                    break;
                }
            }
        }
    }
    ;

const_declaration
    : CONST type IDF '=' constant ';' {
        int idx = insererSymbole($3, $2, "CONSTANT", currentScope, yylineno);
        if (idx != -1) {
            // Check type compatibility
            if (strcmp($2, $5.type) != 0) {
                char errorMsg[100];
                sprintf(errorMsg, "Erreur : Type incompatible pour la constante '%s'. Déclaré : %s, donné : %s.", 
                        $3, $2, $5.type);
                yyerror(errorMsg);
            } else {
                // Initialize the constant with its value
                setConstanteValeur(idx, $2, $5.value);
            }
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
            int erreur=1;
            // Type compatibility check
            if (strcmp(ts.table[idx].type, $3.type) != 0) {
                char errorMsg[100];
                sprintf(errorMsg, "Type incompatible pour la variable '%s'. Déclaré : %s, donné : %s.", 
                        ts.table[idx].nom, ts.table[idx].type, $3.type);
                yyerror(errorMsg);
                erreur = -1;
            }

            if(erreur != -1){
                // Character type special handling
                if (strcmp(ts.table[idx].type, "CHAR") == 0) {
                    if (strlen($3.value) > 1) {
                        printf("Avertissement ligne %d: La variable CHAR '%s' ne peut contenir qu'un seul caractère. Seul le premier caractère sera utilisé.\n", 
                            yylineno, $1);
                    }
                    // Take only the first character
                    ts.table[idx].valeur.caractere = $3.value[1];
                }


                // Assign value based on type
                if (strcmp(ts.table[idx].type, "INTEGER") == 0) {
                    ts.table[idx].valeur.entier = atoi($3.value);
                } else if (strcmp(ts.table[idx].type, "FLOAT") == 0) {
                    ts.table[idx].valeur.reel = atof($3.value);
                }
                // Generate quadruplet
                insererQuadComplet("=", $3.temp, "", $1);
                ts.table[idx].initialise = 1;
            }
        }
    }
    | array_elem '=' expression ';' {
        int idx = rechercherSymbole($1.temp);
        if (idx == -1) {
            yyerror("Tableau non déclaré");
        } else {
            int erreur=1;
            // Type compatibility check
            if (strcmp(ts.table[idx].type, $3.type) != 0) {
                char errorMsg[100];
                sprintf(errorMsg, "Type incompatible pour la variable '%s'. Déclaré : %s, donné : %s.", 
                        ts.table[idx].nom, ts.table[idx].type, $3.type);
                yyerror(errorMsg);
                erreur = -1;
            }

            if(erreur != -1){
                // Character type special handling
                if (strcmp(ts.table[idx].type, "CHAR") == 0) {
                    if (strlen($3.value) > 1) {
                        printf("Avertissement ligne %d: La variable CHAR '%s' ne peut contenir qu'un seul caractère. Seul le premier caractère sera utilisé.\n", 
                            yylineno, $1);
                    }
                    // Take only the first character
                    ts.table[idx].valeur.caractere = $3.value[1];
                }


                // Assign value based on type
                if (strcmp(ts.table[idx].type, "INTEGER") == 0) {
                    ts.table[idx].valeur.entier = atoi($3.value);
                } else if (strcmp(ts.table[idx].type, "FLOAT") == 0) {
                    ts.table[idx].valeur.reel = atof($3.value);
                }
                // Generate quadruplet
                insererQuadComplet("ARRAY_ASSIGN", $3.temp, "", $1.temp);
                ts.table[idx].initialise = 1;
            }
        }
    }
    ;

array_elem
    : IDF '[' expression ']' {
        int baseIdx = rechercherSymbole($1);
        if (baseIdx == -1) {
            yyerror("Tableau non déclaré");
            $$.temp = newTemp();
            $$.type = "INTEGER";
            $$.value = strdup("0");
        } else if (strcmp(ts.table[baseIdx].nature, "TABLEAU") != 0) {
            yyerror("Identificateur n'est pas un tableau");
            $$.temp = newTemp();
            $$.type = strdup(ts.table[baseIdx].type);
            $$.value = strdup("0");
        } else {
            // Convert expression to integer index
            int indexValue = atoi($3.value);
            
            // Check array bounds with precise naming
            char elemName[MAX_NAME_LENGTH];
            sprintf(elemName, "%s[%d]", $1, indexValue);
            
            int elemIdx = rechercherSymbole(elemName);
            if (elemIdx == -1) {
                yyerror("Élément de tableau non déclaré");
                elemIdx = insererElementTableau($1, indexValue, 
                                                ts.table[baseIdx].type, 
                                                currentScope, yylineno);
            }

            $$.temp = strdup(elemName);
            $$.type = strdup(ts.table[baseIdx].type);
            $$.array_size = indexValue;

            // Add quadruplet for array access
            insererQuadComplet("ARRAY_ACCESS", $1, $3.value, $$.temp);
        }
    }
    ;

if_statement
    : IF '(' condition ')' {
        // Sauvegarder la position actuelle pour le branchement conditionnel
        pushBranch(numQuad - 1, numQuad, -1);  // -1 pour end (à remplir plus tard)
        
        // Générer le saut conditionnel
        insererQuadComplet("BZ", $3, "", "?");
    } 
    '{' instructions '}' {
        BranchInfo* current = topBranch();
        
        // Générer le saut inconditionnel pour sauter la partie ELSE
        int jumpToEnd = numQuad;
        insererQuadComplet("BR", "", "", "?");
        
        // Mettre à jour le saut conditionnel avec l'adresse du ELSE
        updateJumpAddress(current->condition, numQuad);
        
        // Sauvegarder la position pour la mise à jour finale
        current->end = jumpToEnd;
    }
    else_part
    ;

else_part
    : /* vide */ {
        BranchInfo* current = topBranch();
        // Mettre à jour le saut à la fin du IF
        updateJumpAddress(current->end, numQuad);
        popBranch();
    }
    | ELSE '{' instructions '}' {
        BranchInfo* current = topBranch();
        // Mettre à jour le saut à la fin du ELSE
        updateJumpAddress(current->end, numQuad);
        popBranch();
    }
    ;

for_statement : FOR '(' for_init ':' {
    // Save start of loop
    pushBranch(numQuad, -1, -1);
} for_step ':' expression ')' {
    BranchInfo* current = topBranch();
    LoopInfo* currentLoop = getCurrentLoop();
    
    // Compare loop var with end expression
    char temp[20];
    sprintf(temp, "t%d", tempCounter++);
    
    // Use GE operator for loop condition
    insererQuadComplet("GE", currentLoop->loopVar, $8.temp, temp);
    
    // Store condition position
    current->condition = numQuad;
    insererQuadComplet("BNZ", temp, "", "");
} '{' instructions '}' {
    BranchInfo* current = topBranch();
    LoopInfo* currentLoop = getCurrentLoop();
    
    // Add step value to loop variable
    char stepTemp[20];
    sprintf(stepTemp, "t%d", tempCounter++);
    insererQuadComplet("+", currentLoop->loopVar, currentLoop->stepValue, stepTemp);
    insererQuadComplet("=", stepTemp, "", currentLoop->loopVar);
    
    // Jump back to condition
    char condTemp[20];
    sprintf(condTemp, "%d", current->condition);
    insererQuadComplet("BR", "", "", condTemp);
    
    // Update branch target
    char endTemp[20];
    sprintf(endTemp, "%d", numQuad);
    updateQuad(current->condition, endTemp, 3);
    
    // Clean up current loop
    popLoop();
    popBranch();
} ;

for_init : IDF '=' expression {
    int idx = rechercherSymbole($1);
    if (idx == -1) {
        yyerror("Variable non déclarée");
    } else {
        // Check if the initialization expression is of type INTEGER
        if (strcmp($3.type, "INTEGER") != 0) {
            char errorMsg[100];
            sprintf(errorMsg, "La valeur d'initialisation de la boucle FOR doit être de type INTEGER, trouvé: %s", $3.type);
            yyerror(errorMsg);
        } else {
            // Now we can assign the integer expression to the loop variable
            insererQuadComplet("=", $3.temp, "", $1);
            ts.table[idx].initialise = 1;
            pushLoop($1);
        }
    }
};


for_step : expression {
    // Check if step value is of type INTEGER
    if (strcmp($1.type, "INTEGER") != 0) {
        yyerror("Le pas de la boucle FOR doit être de type INTEGER");
    } else {
        LoopInfo* currentLoop = getCurrentLoop();
        currentLoop->stepValue = strdup($1.temp);
    }
} ;

io_statement
    : READ '(' IDF ')' ';' {
        int idx = rechercherSymbole($3);
        if (idx == -1) {
            yyerror("Variable non déclarée");
        } else {
            ts.table[idx].initialise = 1;
            insererQuadComplet("READ", "", "", $3);
        }
    }
    | READ '(' IDF '[' CONST_INT ']' ')' ';'{
        int idx = rechercherSymbole($3);
        if (idx == -1) {
            yyerror("Tableau non déclaré");
        } else {
            idx = idx + $5 + 1;
            /*$$.temp = strdup($3);
            $$.type = strdup(ts.table[idx].type);
            
            // Create a string representation of the value
            $$.value = malloc(50);  // Allocate enough memory
            if (strcmp($$.type, "INTEGER") == 0) {
                sprintf($$.value, "%d", ts.table[idx].valeur.entier);
            } else if (strcmp($$.type, "FLOAT") == 0) {
                sprintf($$.value, "%f", ts.table[idx].valeur.reel);
            } else if (strcmp($$.type, "CHAR") == 0) {
                sprintf($$.value, "'%c'", ts.table[idx].valeur.caractere);
            } else {
                strcpy($$.value, "0");
            }*/
            ts.table[idx].initialise = 1;
            insererQuadComplet("READ", "", "", $3);
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
        $$.value = malloc(20);  // Allocate memory for value
        sprintf($$.value, "%d", $1);
    }
    | CONST_FLOAT {
        $$.temp = newTemp();
        char val[20];
        sprintf(val, "%f", $1);
        insererQuadComplet("=", val, "", $$.temp);
        $$.type = "FLOAT";
        $$.value = malloc(20);  // Allocate memory for value
        sprintf($$.value, "%f", $1);
    }
    | CONST_CHAR {
        $$.temp = newTemp();
        char val[20];
        sprintf(val, "'%c'", $1);
        insererQuadComplet("=", val, "", $$.temp);
        $$.type = "CHAR";
        $$.value = malloc(20);  // Allocate memory for value
        sprintf($$.value, "'%c'", $1);
    } 
    | IDF {
        int idx = rechercherSymbole($1);
        if (idx == -1) {
            yyerror("Variable non déclarée");
            $$.temp = newTemp();
            $$.type = "INTEGER";  // Default type
            $$.value = strdup("0");
        } else if (!ts.table[idx].initialise) {
            yyerror("Variable non initialisée");
            $$.temp = newTemp();
            $$.type = strdup(ts.table[idx].type);
            $$.value = strdup("0");
        } else {
            $$.temp = strdup($1);
            $$.type = strdup(ts.table[idx].type);
            
            // Create a string representation of the value
            $$.value = malloc(50);  // Allocate enough memory
            if (strcmp($$.type, "INTEGER") == 0) {
                sprintf($$.value, "%d", ts.table[idx].valeur.entier);
            } else if (strcmp($$.type, "FLOAT") == 0) {
                sprintf($$.value, "%f", ts.table[idx].valeur.reel);
            } else if (strcmp($$.type, "CHAR") == 0) {
                sprintf($$.value, "'%c'", ts.table[idx].valeur.caractere);
            } else {
                strcpy($$.value, "0");
            }
        }
    }
    | IDF '[' CONST_INT ']' {
        int idx = rechercherSymbole($1);
        if (idx == -1) {
            yyerror("Tableau non déclaré");
        } else if (! (ts.table[idx].initialise + $3 + 1)) {
            yyerror("Case Tableau non initialisée");
            $$.temp = newTemp();
        } else {
            idx = idx + $3 + 1;
            $$.temp = strdup($1);
            $$.type = strdup(ts.table[idx].type);
            
            // Create a string representation of the value
            $$.value = malloc(50);  // Allocate enough memory
            if (strcmp($$.type, "INTEGER") == 0) {
                sprintf($$.value, "%d", ts.table[idx].valeur.entier);
            } else if (strcmp($$.type, "FLOAT") == 0) {
                sprintf($$.value, "%f", ts.table[idx].valeur.reel);
            } else if (strcmp($$.type, "CHAR") == 0) {
                sprintf($$.value, "'%c'", ts.table[idx].valeur.caractere);
            } else {
                strcpy($$.value, "0");
            }
        }
    }
    | expression '+' expression {
        $$.temp = newTemp();
        insererQuadComplet("+", $1.temp, $3.temp, $$.temp);
        
        // Type determination and value computation
        if (strcmp($1.type, $3.type) == 0) {
            $$.type = strdup($1.type);
            
            // Compute value if possible
            $$.value = malloc(50);
            if (strcmp($1.type, "INTEGER") == 0) {
                int val = atoi($1.value) + atoi($3.value);
                sprintf($$.value, "%d", val);
            } else if (strcmp($1.type, "FLOAT") == 0) {
                float val = atof($1.value) + atof($3.value);
                sprintf($$.value, "%f", val);
            } else {
                strcpy($$.value, "0");
            }
        } else {
            // Type mismatch handling
            $$.type = "FLOAT";  // Default to float for mixed types
            $$.value = strdup("0");
        }
    }
    | expression '-' expression {
        $$.temp = newTemp();
        insererQuadComplet("-", $1.temp, $3.temp, $$.temp);
        
        // Type and value computation similar to '+'
        if (strcmp($1.type, $3.type) == 0) {
            $$.type = strdup($1.type);
            
            $$.value = malloc(50);
            if (strcmp($1.type, "INTEGER") == 0) {
                int val = atoi($1.value) - atoi($3.value);
                sprintf($$.value, "%d", val);
            } else if (strcmp($1.type, "FLOAT") == 0) {
                float val = atof($1.value) - atof($3.value);
                sprintf($$.value, "%f", val);
            } else {
                strcpy($$.value, "0");
            }
        } else {
            $$.type = "FLOAT";
            $$.value = strdup("0");
        }
    }
    | expression '*' expression {
        $$.temp = newTemp();
        insererQuadComplet("*", $1.temp, $3.temp, $$.temp);
        
        // Type and value computation similar to '+'
        if (strcmp($1.type, $3.type) == 0) {
            $$.type = strdup($1.type);
            
            $$.value = malloc(50);
            if (strcmp($1.type, "INTEGER") == 0) {
                int val = atoi($1.value) * atoi($3.value);
                sprintf($$.value, "%d", val);
            } else if (strcmp($1.type, "FLOAT") == 0) {
                float val = atof($1.value) * atof($3.value);
                sprintf($$.value, "%f", val);
            } else {
                strcpy($$.value, "0");
            }
        } else {
            $$.type = "FLOAT";
            $$.value = strdup("0");
        }
    }
    | expression '/' expression {
        // Implement type conversion for integer division
        char* resultType = "FLOAT";  // Default to float for division
        
        // Determine the result type based on input types
        if (strcmp($1.type, "INTEGER") == 0 && strcmp($3.type, "INTEGER") == 0) {
            printf("Warning: Integer division might lose precision. Result will be a float.\n");
        }

        // Check for division by zero
        int is_zero = 0;
        if (strcmp($3.type, "INTEGER") == 0) {
            is_zero = (atoi($3.value) == 0);
        } else if (strcmp($3.type, "FLOAT") == 0) {
            is_zero = (atof($3.value) == 0.0);
        }

        if (is_zero) {
            printf("Warning: Division by zero. Using 1 as denominator.\n");
            $$.temp = newTemp();
            $$.type = strdup(resultType);
            $$.value = strdup("0.0");
            insererQuadComplet("/", $1.temp, "1", $$.temp);
        } else {
            $$.temp = newTemp();
            insererQuadComplet("/", $1.temp, $3.temp, $$.temp);
            
            $$.type = strdup(resultType);
            $$.value = malloc(50);
            
            // Always compute float division
            float val = (strcmp($1.type, "INTEGER") == 0 ? atoi($1.value) : atof($1.value)) /
                        (strcmp($3.type, "INTEGER") == 0 ? atoi($3.value) : atof($3.value));
            sprintf($$.value, "%f", val);
        }
    }
    | '(' expression ')' {
        $$ = $2;
    }
    | '-' expression %prec UNARY_MINUS {
        $$.temp = newTemp();
        insererQuadComplet("*", $2.temp, "-1", $$.temp);
        
        $$.type = $2.type;
        $$.value = malloc(50);
        if (strcmp($$.type, "INTEGER") == 0) {
            int val = -atoi($2.value);
            sprintf($$.value, "%d", val);
        } else if (strcmp($$.type, "FLOAT") == 0) {
            float val = -atof($2.value);
            sprintf($$.value, "%f", val);
        } else {
            strcpy($$.value, "0");
        }
    }
    ;

condition
    : expression comparison_op expression {
        $$ = newTemp();
        char* opcode;
        if (strcmp($1.type, "INTEGER") != 0 && 
            strcmp($1.type, "FLOAT") != 0 && 
            strcmp($1.type, "CHAR") != 0) {
            yyerror("Type invalide pour la comparaison");
        }  
        if (strcmp($2, "==") == 0) opcode = "NE";         // Branch if not equal
        else if (strcmp($2, "!=") == 0) opcode = "EQ";    // Branch if equal
        else if (strcmp($2, "<") == 0) opcode = "GE";     // Branch if greater/equal
        else if (strcmp($2, ">") == 0) opcode = "LE";     // Branch if less/equal
        else if (strcmp($2, "<=") == 0) opcode = "GT";    // Branch if greater
        else if (strcmp($2, ">=") == 0) opcode = "LT";    // Branch if less

        // Generate comparison quadruple
        insererQuadComplet(opcode, $1.temp, $3.temp, $$);
    }
    | condition AND condition {
        $$ = newTemp();
        // For AND, both conditions must be true
        insererQuadComplet("OR", $1, $3, $$);  // Using OR since we're testing for falseness
    }
    | condition OR condition {
        $$ = newTemp();
        insererQuadComplet("AND", $1, $3, $$);  // Using AND since we're testing for falseness
    }
    | NOT condition {
        $$ = $2;  // Simply pass through, BZ will handle the negation
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
    initialiserTS();   // Ensure this function is defined in symbole.h or elsewhere.
    printf("Début de l'analyse...\n");
    yyparse();
    return 0;
}