#ifndef CFONB_TYPES_H_INCLUED
#define CFONB_TYPES_H_INCLUED

// Types d'enregistrements CFONB
typedef enum {
    RECORD_OLD_BALANCE = 1,    // 01 - Ancien solde
    RECORD_OPERATION = 4,      // 04 - Opération
    RECORD_COMPLEMENT = 5,     // 05 - Complément
    RECORD_NEW_BALANCE = 7,     // 07 - Nouveau solde
} RecordType;
// Sens de l'opération
typedef enum {
    SENS_CREDIT,    // Entrée d'argent
    SENS_DEBIT      // Sortie d'argent
} SensOperation;
// Codes retour pour la validation
typedef enum {
    VALID_OK = 0,
    VALID_ERR_SEQUENCE,       // Séquence 01-04-07 non respectée
    VALID_ERR_COMPTE,         // Numéro de compte incohérent
    VALID_ERR_SOLDE,          // Solde recalculé != solde déclaré
    VALID_ERR_FORMAT          // Erreur de format
} ValidationResult;

//LES STRUCTURES PRINCIPALES

typedef struct {
    char commande[20];        // "load", "validate", "stats", "search"
    char fichier[256];        // Chemin du fichier
    char numeroCompte[12];    // Filtre par compte (optionnel)
    long montantMin;          // Filtre montant minimum
    char date[7];             // Filtre par date JJMMAA
    short verbose;            // Mode détaillé
} Arguments;

// Date au format CFONB (JJMMAA)
typedef struct {
    unsigned int jour  : 5;  // 1-31  → max 31, besoin de 5 bits
    unsigned int mois  : 4;  // 1-12  → max 12, besoin de 4 bits
    unsigned int annee : 7;  // 0-99  → max 99, besoin de 7 bits (25 = 2025)
} DateCFONB;  // Total : 16 bits (2 octets)
// Montant avec signe
typedef struct {
    unsigned long centimes;        // Montant en centimes (évite les flottants)
    SensOperation sens;            // Crédit ou débit
} Montant;
// Informations de compte (enregistrements 01 et 07)
typedef struct {
    char codeBanque[6];      // 5 + \0
    char codeGuichet[6];     // 5 + \0
    char devise[4];          // 3 + \0 (EUR)
    int nbDecimales;
    char numeroCompte[12];   // 11 + \0
    DateCFONB date;
    char titulaire[51];      // 50 + \0
    Montant solde;
} InfoCompte;
// Une opération (enregistrement 04)
typedef struct {
    char numeroCompte[12];
    char codeOperation[3];   // 2 + \0
    DateCFONB dateOperation;
    DateCFONB dateValeur;
    char libelle[32];        // 31 + \0
    Montant montant;
    char reference[17];      // 16 + \0
    // Compléments éventuels (04 peut avoir plusieurs 05)
    char complements[10][71]; // Max 5 compléments de 70 chars
    int nbComplements;
} Operation;
// Bloc complet d'un compte (01 + opérations + 07)
typedef struct {
    InfoCompte ancienSolde;    // Enregistrement 01
    Operation* operations;     // Tableau dynamique d'opérations
    int nbOperations;
    int capaciteOperations;    // Pour la gestion dynamique
    InfoCompte nouveauSolde;   // Enregistrement 07
} BlocCompte;
// Fichier CFONB complet
typedef struct {
    char nomFichier[256];
    BlocCompte* blocs;         // Tableau dynamique de blocs
    int nbBlocs;
    int capaciteBlocs;
} FichierCFONB;


//LES STRUCTURES DE RESULTATS

// Statistiques d'un compte
typedef struct {
    char numeroCompte[12];
    char titulaire[51];
    Montant soldeInitial;
    Montant soldeFinal;
    int nbDebits;
    int nbCredits;
    long totalDebits;       // En centimes
    long totalCredits;      // En centimes
    long variation;         // Credits - Debits
} StatsCompte;
// Rapport de validation
typedef struct {
    ValidationResult resultat;
    int ligneErreur;
    char messageErreur[256];
} RapportValidation;
#endif
