#ifndef SYMBOLE_H
#define SYMBOLE_H

#define MAX_SYMBOLES 1000
#define MAX_ARRAY_SIZE 100
#define MAX_NAME_LENGTH 9  // 8 chars + null terminator

typedef union {
    int entier;
    float reel;
    char caractere;
    int tableau_entier[MAX_ARRAY_SIZE];
    float tableau_reel[MAX_ARRAY_SIZE];
    char tableau_caractere[MAX_ARRAY_SIZE];
} Valeur;

typedef struct {
    char nom[MAX_NAME_LENGTH];       
    char type[20];                   
    char nature[20];                 
    char scope[20];                  
    int taille;                      
    int ligne;                        
    int initialise;                   
    Valeur valeur;                    
    
    // Additional metadata for array tracking
    char array_base_name[MAX_NAME_LENGTH];  // Original array name (e.g., Tab)
    int array_index;                        // Index in the original array (-1 for simple vars)
} Symbole;

typedef struct {
    Symbole table[MAX_SYMBOLES];
    int taille;
} TableSymboles;

extern TableSymboles ts;

// Function prototypes
void initialiserTS();
int rechercherSymbole(char* nom);
int insererSymbole(char* nom, char* type, char* nature, char* scope, int ligne);
void modifierTaille(int index, int taille);
int initialiserElementTableau(char* nomTableau, int index, void* valeur);
void* getTableauElement(char* nomTableau, int index);
int estElementTableauInitialise(char* nomTableau, int index);
void afficherTS();
void setConstanteValeur(int index, char* type, void* valeur);
int verifierTypeCompatible(char* type1, char* type2);
int verifierBornesTableau(char* nomTableau, int index);
int insererElementTableau(char* baseNom, int index, char* type, char* scope, int ligne);

#endif // SYMBOLE_H