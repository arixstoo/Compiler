// quadruples.h
#ifndef QUADRUPLES_H
#define QUADRUPLES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUAD 1000

typedef struct qdr {
    char operation[100];
    char operande1[100];
    char operande2[100];
    char res[100];
} qdr;

extern qdr quad[MAX_QUAD];
extern int numQuad;

// Déclarations des fonctions
void ajouterQuad(int num_quad, int colon_quad, char val[]);
void insererQuadComplet(char operation[], char operande1[], char operande2[], char res[]);
void unSeulQuad(qdr elem);
void afficherQuad();
void supprimerQuad(int index);
char* genererTemporaire();
void ecrireQuadDansFichier(const char* nomFichier);
#endif // QUADRUPLES_H