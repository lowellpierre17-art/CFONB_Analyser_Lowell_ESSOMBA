#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 #include "cfonb_parser.h"
#include "cfonb_types.h"
#include "cfonb_utils.h"

int main() {
    /*Arguments args;
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
    }*/
    FILE*fp = fopen("C://Users//NICKSON//CLionProjects//CFONB_Analyser_Tchinda_Fotsa_Nickson//file.txt", "r");
    if (fp == NULL)
        printf("Error opening file!\n");
    else {
        char line[120];
        fgets(line, 120, fp);
        printf("%s\n", line);
        char somme[7];
        extraireChamp(line,35,40,somme);
        DateCFONB date = parseDate(somme);
        printf("Date : %d - %d - %d\n",date.jour,date.mois,date.annee);
        extraireChamp(line,91,104,somme);
        printf("%s\n",somme);
        Montant montant = decoderMontant(somme,2);
        afficherMontant(montant);
    }
    return 0;

}
