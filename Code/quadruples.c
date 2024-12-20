#include "quadruples.h"
#include "symbole.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

qdr quad[MAX_QUAD];
int numQuad = 0;
int tempCounter = 0;

void ajouterQuad(int num_quad, int colon_quad, char val[]) {
    if(num_quad >= MAX_QUAD || num_quad >= numQuad) {
        printf("Erreur: Position de quadruplet invalide\n");
        return;
    }

    switch (colon_quad) {
        case 1: 
            strcpy(quad[num_quad].operation, val);
            break;
        case 2:
            strcpy(quad[num_quad].operande1, val);
            break;
        case 3:
            strcpy(quad[num_quad].operande2, val);
            break;
        case 4:
            strcpy(quad[num_quad].res, val);
            // If this is updating a branch target, verify it's a valid target
            if (strcmp(quad[num_quad].operation, "BZ") == 0 ||
                strcmp(quad[num_quad].operation, "BR") == 0) {
                int target = atoi(val);
                if (target < 0 || target > numQuad) {
                    printf("Erreur: Cible de branchement invalide %d\n", target);
                    return;
                }
            }
            break;
    }
}

void updateQuad(int position, char* value, int field) {
    // field: 0 for operation, 1 for operande1, 2 for operande2, 3 for res
    switch(field) {
        case 0: strcpy(quad[position].operation, value); break;
        case 1: strcpy(quad[position].operande1, value); break;
        case 2: strcpy(quad[position].operande2, value); break;
        case 3: strcpy(quad[position].res, value); break;
    }
}


void insererQuadComplet(char operation[], char operande1[], char operande2[], char res[]) {
    if(numQuad >= MAX_QUAD) {
        printf("Erreur: Dépassement de la capacité des quadruplets\n");
        return;
    }

    strcpy(quad[numQuad].operation, operation);
    strcpy(quad[numQuad].operande1, operande1);
    strcpy(quad[numQuad].operande2, operande2);
    strcpy(quad[numQuad].res, res);
    numQuad++;
}

void unSeulQuad(qdr elem) {
    printf(" %8s | %8s | %8s | %8s \n", 
        elem.operation, 
        elem.operande1, 
        elem.operande2, 
        elem.res);
}

void afficherQuad() {
    printf("\n\n\tLES QUADRUPLETS\n\t===============\n");
    printf(" %8s | %8s | %8s | %8s \n",
           "OPERATION",
           "OPERANDE1",
           "OPERANDE2",
           "RESULTAT");
    printf("------------------------------------------\n");
    
    for(int i = 0; i < numQuad; i++) {
        printf("%d- ", i);
        unSeulQuad(quad[i]);
    }
    printf("==========================================\n");
}

char* genererTemporaire() {
    char* temp = malloc(10);
    sprintf(temp, "T%d", tempCounter++);
    return temp;
}