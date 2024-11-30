#include "symbole.h"

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

// Insertion d'un nouveau symbole
int insererSymbole(char* nom, char* type, char* nature, char* scope, int ligne) {
    if(ts.taille >= MAX_SYMBOLES) {
        printf("Erreur: Table des symboles pleine\n");
        return -1;
    }
    
    if(strlen(nom) > 8) {
        printf("Erreur ligne %d: Nom d'identificateur '%s' trop long (max 8 caractères)\n", ligne, nom);
        return -1;
    }
    
    int index = rechercherSymbole(nom);
    if(index != -1 && strcmp(ts.table[index].scope, scope) == 0) {
        printf("Erreur ligne %d: Double déclaration de '%s'\n", ligne, nom);
        return -1;
    }
    
    strcpy(ts.table[ts.taille].nom, nom);
    strcpy(ts.table[ts.taille].type, type);
    strcpy(ts.table[ts.taille].nature, nature);
    strcpy(ts.table[ts.taille].scope, scope);
    ts.table[ts.taille].initialise = 0;
    ts.table[ts.taille].taille = 0;
    ts.table[ts.taille].ligne = ligne;
    
    return ts.taille++;
}

// Modification du type d'un symbole
void modifierType(int index, char* type) {
    if(index >= 0 && index < ts.taille) {
        strcpy(ts.table[index].type, type);
    }
}

// Modification de la taille pour les tableaux
void modifierTaille(int index, int taille) {
    if(index >= 0 && index < ts.taille) {
        ts.table[index].taille = taille;
    }
}

// Définir une valeur pour une constante
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

// Vérification de la compatibilité des types
int verifierTypeCompatible(char* type1, char* type2) {
    return strcmp(type1, type2) == 0;
}

// Affichage de la table des symboles
void afficherTS() {
    printf("\n=== TABLE DES SYMBOLES ===\n");
    printf("%-8s | %-8s | %-12s | %-8s | %-8s | %-10s | %-6s\n", 
           "NOM", "TYPE", "NATURE", "TAILLE", "INIT", "SCOPE", "LIGNE");
    printf("------------------------------------------------------------------\n");
    
    for(int i = 0; i < ts.taille; i++) {
        printf("%-8s | %-8s | %-12s | %-8d | %-8d | %-10s | %-6d\n",
               ts.table[i].nom,
               ts.table[i].type,
               ts.table[i].nature,
               ts.table[i].taille,
               ts.table[i].initialise,
               ts.table[i].scope,
               ts.table[i].ligne);
    }
    printf("=== FIN TABLE DES SYMBOLES ===\n");
}
