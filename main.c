#include <stdio.h>
#include <string.h>

#include "cfonb_parser.h"
#include "cfonb_types.h"
#include "cfonb_utils.h"

int main(int argc, char* argv[]) {
    Arguments args;
    int resultat = parserArguments(argc, argv, &args);
    if (resultat <= 0) {
        afficherUsage(argv[0]);
        return (resultat < 0) ? 1 : 0;
    }
    // Exécuter la commande demandée
    if (strcmp(args.commande, "load") == 0) {
        // chargerEtAfficher(args.fichier);
        printf("Chargement de %s...\n", args.fichier);
    }
    else if (strcmp(args.commande, "validate") == 0) {
        // validerFichier(args.fichier);
        printf("Validation de %s...\n", args.fichier);
    }
    else if (strcmp(args.commande, "stats") == 0) {
        // afficherStats(args.fichier);
        printf("Statistiques de %s...\n", args.fichier);
    }
    else if (strcmp(args.commande, "search") == 0) {
        // rechercherOperations(args.fichier, &args);
        printf("Recherche dans %s...\n", args.fichier);
        if (args.numeroCompte[0] != '\0') {
            printf("  Filtre compte : %s\n", args.numeroCompte);
        }
    }
    else {
        printf("Erreur : commande '%s' inconnue\n", args.commande);
        afficherUsage(argv[0]);
        return 1;
    }
    return 0;

}
