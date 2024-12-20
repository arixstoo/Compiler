#ifndef QUADRUPLES_H
#define QUADRUPLES_H

#define MAX_QUAD 1000
#define MAX_STR_LEN 50

typedef struct {
    char operation[MAX_STR_LEN];
    char operande1[MAX_STR_LEN];
    char operande2[MAX_STR_LEN];
    char res[MAX_STR_LEN];
} qdr;

extern qdr quad[MAX_QUAD];
extern int numQuad;
extern int tempCounter;

void ajouterQuad(int num_quad, int colon_quad, char val[]);
void insererQuadComplet(char operation[], char operande1[], char operande2[], char res[]);
void unSeulQuad(qdr elem);
void afficherQuad();
char* genererTemporaire();
void updateQuad(int position, char* value, int field);
int verifierBornesTableau(char* nomTableau, int index);

#endif