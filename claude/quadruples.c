#include "quadruples.h"

qdr quad[MAX_QUAD];
int numQuad = 0;
int tempCounter = 0;

void ajouterQuad(int num_quad, int colon_quad, char val[]) {
    if(num_quad >= MAX_QUAD) {
        printf("Erreur: Dépassement de la capacité des quadruplets\n");
        return;
    }

    if (num_quad >= numQuad) {
        printf("Erreur : quadruple inexistant à l'index %d\n", num_quad);
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
            break;
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

void supprimerQuad(int index) {
    if (index < 0 || index >= numQuad) {
        printf("Erreur : Index invalide pour suppression\n");
        return;
    }
    for (int i = index; i < numQuad - 1; i++) {
        quad[i] = quad[i + 1];
    }
    numQuad--;
}

char* genererTemporaire() {
    char* temp = malloc(10);
    sprintf(temp, "T%d", tempCounter++);
    return temp;
}

void ecrireQuadDansFichier(const char* nomFichier) {
    FILE* file = fopen(nomFichier, "w");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", nomFichier);
        return;
    }
    fprintf(file, "OPERATION | OPERANDE1 | OPERANDE2 | RESULTAT\n");
    for (int i = 0; i < numQuad; i++) {
        fprintf(file, "%s | %s | %s | %s\n",
                quad[i].operation, quad[i].operande1,
                quad[i].operande2, quad[i].res);
    }
    fclose(file);
}