#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfonb_parser.h"
#include "cfonb_stats.h"
#include "cfonb_types.h"
#include "cfonb_utils.h"
#include "cfonb_validator.h"

int main(int argc, char* argv[]) {
    Arguments args;
    int resultat = parserArguments(argc, argv, &args);
    if (resultat <= 0) {
        afficherUsage(argv[0]);
        return (resultat < 0) ? 1 : 0;
    }
    afficherArguments(&args);
    printf("\n");
    // Exécuter la commande demandée
    if (strcmp(args.commande, "load") == 0) {
        FichierCFONB*fichier = chargerFichier(args.fichier);
        if (!fichier) printf("Echec de chargement\n");
        else
        chargerficher(fichier);
    }
    else if (strcmp(args.commande, "validate") == 0) {
        afficherValiderFichier(args.fichier);
    }
    else if (strcmp(args.commande, "stats") == 0) {
        afficherStats(args.fichier);
    }
    else if (strcmp(args.commande, "search") == 0) {
        afficherRechercherOperations(args.fichier, &args);
    }
    else {
        printf("Erreur : commande '%s' inconnue\n", args.commande);
        afficherUsage(argv[0]);
        return 1;
    }
    return 0;
}