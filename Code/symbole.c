

#include "symbole.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TableSymboles ts;


void initialiserTS() {
    ts.taille = 0;
}

int rechercherSymbole(char* nom) {  
    for(int i = 0; i < ts.taille; i++) {
        if(strcmp(ts.table[i].nom, nom) == 0) {
            return i;
        }
    }
    return -1;
}

int insererSymbole(char* nom, char* type, char* nature, char* scope, int ligne) {
    if (ts.taille >= MAX_SYMBOLES) {
        printf("Erreur: Table des symboles pleine\n");
        return -1;
    }
    
    if (strlen(nom) > 8) {
        printf("Erreur ligne %d: Nom d'identificateur '%s' trop long (max 8 caractères)\n", ligne, nom);
        return -1;
    }

    // Check for duplicate 
    for (int i = 0; i < ts.taille; i++) {
        if (strcmp(ts.table[i].nom, nom) == 0) {
            printf("Erreur ligne %d: Double déclaration de '%s'\n", ligne, nom);
            return -1;
        }
    }
    
    // Special handling for array declaration
    if (strcmp(nature, "TABLEAU") == 0) {
        // Note: Actual array elements will be inserted separately in syn.y
        strcpy(ts.table[ts.taille].nom, nom);
        strcpy(ts.table[ts.taille].type, type);
        strcpy(ts.table[ts.taille].nature, nature);
        strcpy(ts.table[ts.taille].scope, scope);
        ts.table[ts.taille].ligne = ligne;
        ts.table[ts.taille].taille = 1;  // Will be modified later
        ts.table[ts.taille].initialise = 0;
        strcpy(ts.table[ts.taille].array_base_name, nom);
        ts.table[ts.taille].array_index = -1;  // Base array marker
        
        return ts.taille++;
    }
    
    // Regular variable insertion
    strcpy(ts.table[ts.taille].nom, nom);
    strcpy(ts.table[ts.taille].type, type);
    strcpy(ts.table[ts.taille].nature, nature);
    strcpy(ts.table[ts.taille].scope, scope);
    ts.table[ts.taille].ligne = ligne;
    ts.table[ts.taille].taille = 1;
    ts.table[ts.taille].initialise = 0;
    strcpy(ts.table[ts.taille].array_base_name, "");
    ts.table[ts.taille].array_index = -1;

    return ts.taille++;
}

int insererElementTableau(char* baseNom, int index, char* type, char* scope, int ligne) {
    // Créer un nom pour l'élément du tableau
    char nomElement[MAX_NAME_LENGTH];
    sprintf(nomElement, "%s[%d]", baseNom, index);

    if (ts.taille >= MAX_SYMBOLES) {
        printf("Erreur: Table des symboles pleine\n");
        return -1;
    }

    // Check for duplicate
    for (int i = 0; i < ts.taille; i++) {
        if (strcmp(ts.table[i].nom, nomElement) == 0) {
            printf("Erreur ligne %d: Double déclaration de '%s'\n", ligne, nomElement);
            return -1;
        }
    }

    // Insérer l'élément du tableau
    strcpy(ts.table[ts.taille].nom, nomElement);
    strcpy(ts.table[ts.taille].type, type);
    strcpy(ts.table[ts.taille].nature, "VARIABLE");
    strcpy(ts.table[ts.taille].scope, scope);
    ts.table[ts.taille].ligne = ligne;
    ts.table[ts.taille].taille = 1;
    ts.table[ts.taille].initialise = 0;
    strcpy(ts.table[ts.taille].array_base_name, baseNom);
    ts.table[ts.taille].array_index = index;

    return ts.taille++;
}

void setConstanteValeur(int index, char* type, void* valeur) {
    if(index >= 0 && index < ts.taille) {
        if(strcmp(type, "INTEGER") == 0) {
            ts.table[index].valeur.entier = *((int*)valeur);
        }
        else if(strcmp(type, "FLOAT") == 0) {
            ts.table[index].valeur.reel = *((float*)valeur);
        }
        else if(strcmp(type, "CHAR") == 0) {
            ts.table[index].valeur.caractere = *((char*)valeur);
        }
        ts.table[index].initialise = 1;
    }
}

void afficherTS() {
    printf("\n=== TABLE DES SYMBOLES ===\n");
    printf("%-15s | %-10s | %-15s | %-10s | %-8s | %-10s | %-6s | %-15s\n", 
           "NOM", "TYPE", "NATURE", "TAILLE", "INIT", "SCOPE", "LIGNE", "VALEUR");
    printf("---------------------------------------------------------------------------------\n");
    
    for(int i = 0; i < ts.taille; i++) {
        char valeurStr[200] = "";
        
        if (ts.table[i].initialise) {
            if (strcmp(ts.table[i].type, "INTEGER") == 0)
                sprintf(valeurStr, "%d", ts.table[i].valeur.entier);
            else if (strcmp(ts.table[i].type, "FLOAT") == 0)
                sprintf(valeurStr, "%f", ts.table[i].valeur.reel);
            else if (strcmp(ts.table[i].type, "CHAR") == 0)
                sprintf(valeurStr, "'%c'", ts.table[i].valeur.caractere);
        } else {
            strcpy(valeurStr, "non initialisé");
        }

        printf("%-15s | %-10s | %-15s | %-10d | %-8d | %-10s | %-6d | %-15s\n",
               ts.table[i].nom,
               ts.table[i].type,
               ts.table[i].nature,
               ts.table[i].taille,
               ts.table[i].initialise,
               ts.table[i].scope,
               ts.table[i].ligne,
               valeurStr);
    }
    printf("=== FIN TABLE DES SYMBOLES ===\n");
}

void modifierTaille(int index, int taille) {
    if(index >= 0 && index < ts.taille) {
        // Mettre à jour la taille du tableau
        ts.table[index].taille = taille;
    }
}

int verifierTypeCompatible(char* type1, char* type2) {
    return strcmp(type1, type2) == 0;
}

int verifierBornesTableau(char* nomTableau, int index) {
    int idx = rechercherSymbole(nomTableau);
    if(idx == -1) return 0;
    
    if(strcmp(ts.table[idx].nature, "TABLEAU") != 0) return 0;
    
    if(index < 0 || index >= ts.table[idx].taille) {
        printf("Warning: Index %d out of bounds for array %s\n", 
               index, nomTableau);
        return 0;
    }
    return 1;
}

int initialiserElementTableau(char* nomTableau, int index, void* valeur) {
    int idx = rechercherSymbole(nomTableau);
    if(idx == -1) {
        printf("Erreur : Tableau %s non trouvé\n", nomTableau);
        return 0;
    }

    // Vérification stricte des bornes
    if(index < 0 || index >= ts.table[idx].taille) {
        printf("Erreur : Index %d hors des bornes du tableau %s (taille : %d)\n", 
               index, nomTableau, ts.table[idx].taille);
        return 0;
    }

    // Initialisation spécifique au type
    if(strcmp(ts.table[idx].type, "INTEGER") == 0) {
        ts.table[idx].valeur.tableau_entier[index] = *((int*)valeur);
        
        // Marquer cet élément comme initialisé
        ts.table[idx].initialise = 1;
    } 
    else if(strcmp(ts.table[idx].type, "FLOAT") == 0) {
        ts.table[idx].valeur.tableau_reel[index] = *((float*)valeur);
        
        // Marquer cet élément comme initialisé
        ts.table[idx].initialise = 1;
    } 
    else if(strcmp(ts.table[idx].type, "CHAR") == 0) {
        ts.table[idx].valeur.tableau_caractere[index] = *((char*)valeur);
        
        // Marquer cet élément comme initialisé
        ts.table[idx].initialise = 1;
    } 
    else {
        printf("Erreur : Type de tableau non reconnu\n");
        return 0;
    }


    return 1;
}
