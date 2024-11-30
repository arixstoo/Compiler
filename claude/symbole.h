// symbole.h
#ifndef SYMBOLE_H
#define SYMBOLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOLES 1000

typedef struct {
    char nom[9];
    char type[10];
    char nature[15];
    int taille;
    union {
        int entier;
        float reel;
        char caractere;
        char* chaine;
    } valeur;
    int initialise;
    char scope[20];
    int ligne;
} Symbole;

typedef struct {
    Symbole table[MAX_SYMBOLES];
    int taille;
} TableSymboles;

extern TableSymboles ts;

// Déclarations des fonctions
void initialiserTS();
int rechercherSymbole(char* nom);
int insererSymbole(char* nom, char* type, char* nature, char* scope, int ligne);
void modifierType(int index, char* type);
void modifierTaille(int index, int taille);
void setConstanteValeur(int index, char* type, void* valeur);
int verifierTypeCompatible(char* type1, char* type2);
void afficherTS();

#endif // SYMBOLE_H
