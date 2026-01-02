#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfonb_utils.h"
#include "cfonb_types.h"

void afficherUsage(const char *nomProgramme) {
    printf("Usage : %s <commande> <fichier> [options]\n\n", nomProgramme);
    printf("Commandes :\n");
    printf("  load <fichier>       Charger et afficher un résumé\n");
    printf("  validate <fichier>   Valider la cohérence des données\n");
    printf("  stats <fichier>      Afficher les statistiques\n");
    printf("  search <fichier>     Rechercher des opérations\n\n");
    printf("Options pour 'search' :\n");
    printf("  --compte <num>       Filtrer par numéro de compte\n");
    printf("  --montant-min <n>    Montant minimum en centimes\n");
    printf("  --date <JJMMAA>      Filtrer par date\n\n");
    printf("Options générales :\n");
    printf("  --verbose, -v        Affichage détaillé\n");
    printf("  --help, -h           Afficher cette aide\n");
}

//il me faudra rajouter le fait que si le numero de compte n'a pas été ajouté, alors j'attribue NULL à cette chaine
void afficherArgument(Arguments * args) {
    // Afficher ce qu'on a compris
    if (!args) printf("Argument vide");
    else {
        printf("Commande : %s\n", args->commande);
        printf("Fichier : %s\n", args->fichier);
        printf("Filtre compte : %s\n", (strlen(args->numeroCompte)!=11) ? args->numeroCompte : "(aucun)");
        printf("Montant min : %ld\n", args->montantMin);
        printf("Mode verbose : %s\n", args->verbose ? "oui" : "non");
    }
}

// Conversion du caractère signé en montant
Montant decoderMontant(const char* montantStr, int nbDecimales) {
    Montant m;
    int longueur = (int)strlen(montantStr);  // 14 pour types 01/07, 13 pour type 04
    char dernierChar = montantStr[longueur - 1];
    // Extraire les (longueur-1) premiers caractères comme nombre
    char partie[15];
    strncpy(partie, montantStr, longueur-1);
    partie[longueur - 1] = '\0';
    long valeur = atol(partie) * 10;  // Multiplier par 10 pour le dernier chiffre
    m.sens = SENS_CREDIT;//Pour palier à une absence de non-initialisation
    // Décoder le dernier caractère
    if (dernierChar == '{') {
        m.sens = SENS_CREDIT;
        valeur += 0;
    } else if (dernierChar >= 'A' && dernierChar <= 'I') {
        m.sens = SENS_CREDIT;
        valeur += (dernierChar - 'A' + 1);
    } else if (dernierChar == '}') {
        m.sens = SENS_DEBIT;
        valeur += 0;
    } else if (dernierChar >= 'J' && dernierChar <= 'R') {
        m.sens = SENS_DEBIT;
        valeur += (dernierChar - 'J' + 1);
    }

    // Normaliser en centimes (Déjà en centimes si nbDecimales = 2)
    if (nbDecimales == 0) {
        valeur *= 100;  // Pas de décimales → multiplier par 100
    } else if (nbDecimales == 1) {
        valeur *= 10;   // 1 décimale → multiplier par 10
    }
    m.centimes = valeur;
    return m;
}
// Parsing d'une date JJMMAA
DateCFONB parseDate(const char* dateStr) {
    DateCFONB date;
    char nombre[2];
    strncpy(nombre, dateStr, 2);
    date.jour = atoi(nombre);
    strncpy(nombre, dateStr + 2, 2);
    date.mois = atoi(nombre);
    strncpy(nombre, dateStr + 4, 2);
    date.annee = atoi(nombre);
    return date;
}
// Extraction d'une sous-chaîne (positions CFONB sont en base 1)
void extraireChamp(const char* ligne, int debut, int fin, char* dest) {
    int longueur = fin - debut + 1;
    strncpy(dest, ligne + debut - 1, longueur);
    dest[longueur] = '\0';
    // Retirer les espaces de fin (trim)
    while (longueur > 0 && dest[longueur - 1] == ' ') {
        dest[--longueur] = '\0';
    }
}
// Affichage formaté d'un montant
void afficherMontant(Montant m) {
    double valeur = (double)m.centimes/100;
    printf("%.2f € %s\n",valeur,(m.sens == SENS_CREDIT) ? "(CR)" : "(DB)");

}
// Comparaison de dates
//cette fonction retourne 0 si la date d1 est superieur ou égale à d2 et 1 dans le cas inverse
int comparerDates(DateCFONB d1, DateCFONB d2) {
    return(d1.annee==d2.annee && d1.mois == d2.mois && d1.jour == d2.jour)? 0:1;
}

// Initialisation d'un fichier CFONB
FichierCFONB* creerFichier(const char* nom) {
    FichierCFONB* f = malloc(sizeof(FichierCFONB));
    if (!f) return NULL;
    strncpy(f->nomFichier, nom, 255);
    f->nomFichier[255] = '\0';
    f->capaciteBlocs = 10;  // Capacité initiale
    f->blocs = malloc(f->capaciteBlocs * sizeof(BlocCompte));
    f->nbBlocs = 0;
    return f;
}
// Ajouter un bloc (avec réallocation si nécessaire)
int ajouterBloc(FichierCFONB* fichier, BlocCompte bloc) {
    if (fichier->nbBlocs >= fichier->capaciteBlocs) {
        fichier->capaciteBlocs *= 2;
        BlocCompte* nouveau = realloc(fichier->blocs,fichier->capaciteBlocs * sizeof(BlocCompte));
        if (!nouveau) return -1;
        fichier->blocs = nouveau;
    }
    fichier->blocs[fichier->nbBlocs++] = bloc;
    return 0;
}
//Initialisation d'un bloc pour l'allocation dynamique
BlocCompte* creerBloc() {
    BlocCompte* bloc = malloc(sizeof(BlocCompte));
    if (!bloc) return NULL;
    bloc->capaciteOperations = 10;
    bloc->operations = malloc(bloc->capaciteOperations * sizeof(Operation));
    bloc->nbOperations = 0;
    return bloc;
}
//Ajouter une opération à un bloc
int ajouterOperation(BlocCompte* bloc, Operation operation) {
    if (bloc->nbOperations >= bloc->capaciteOperations) {
        bloc->capaciteOperations *= 2;
        Operation* nouveau = realloc(bloc->operations,bloc->capaciteOperations * sizeof(Operation));
        if (!nouveau) return -1;
        bloc->operations = nouveau;
    }
    bloc->operations[bloc->nbOperations++] = operation;
    return 0;
}