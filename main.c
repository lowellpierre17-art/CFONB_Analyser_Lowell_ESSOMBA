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

        InfoCompte*info = (InfoCompte*)malloc(sizeof(InfoCompte));
        Operation*op = (Operation*)malloc(sizeof(Operation));
        if (parseInfoCompte(line,info))
            printf("Erreur de parsing");
        else {
            printf("code banque : %s\n",info->codeBanque);
            printf("code guichet : %s\n",info->codeGuichet);
            printf("Numero de compte : %s\n",info->numeroCompte);
            printf("Date : %d - %d - %d\n",info->date.jour,info->date.mois,info->date.annee);
            printf("titulaire : %s\n",info->titulaire);
            afficherMontant(info->solde);
            printf("devise : %s\n",info->devise);
            fgets(line, 120, fp);
        }
        if (parseOperation(line,op,fp))
            printf("Erreur de parsing");
        else {
            printf("\n\n-----------------operation-04--------------------\n\n");
            printf("%s\n", line);
            printf("numero de compte : %s\n",op->numeroCompte);
            printf("Code operation : %s\n",op->codeOperation);
            printf("Date operation : %d - %d - %d\n",op->dateOperation.jour,op->dateOperation.mois,op->dateOperation.annee);
            printf("Date valeur : %d - %d - %d\n",op->dateValeur.jour,op->dateValeur.mois,op->dateValeur.annee);
            printf("Libelle : %s\n",op->libelle);
            afficherMontant(op->montant);
            printf("reference : %s\n",op->reference);
            for (int i = 0;i<op->nbComplements;i++)
                printf("complement : %s\n",op->complements[i]);

        }
        fgets(line, 120, fp);
        printf("%s\n", line);
        }
    return 0;

}