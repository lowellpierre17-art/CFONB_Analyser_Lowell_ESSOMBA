#include <stdio.h>
#include "cfonb_parser.h"
#include "cfonb_types.h"
#include "cfonb_utils.h"
#include "cfonb_validator.h"

int main() {

    FichierCFONB* fichier = chargerFichier("D://data//file (13).txt");
    if (fichier == NULL)
        printf("Error parsing file!\n");
    else {
            for (int k=0;k<fichier->nbBlocs;++k) {
                RapportValidation rapport = validerStructureBloc(&fichier->blocs[k]);
                printf("\n\n ~~~~~~~~~~~~~Bloc%d~~~~~~~~~~~~~~~~~~~~~~~~~ : \n\n", k+1);
                printf("\n*****%s*******\n",rapport.messageErreur);
                if (rapport.resultat == VALID_OK) {
                    rapport = validerCoherenceCompte(&fichier->blocs[k]);
                    printf("\n*****%s*******\n",rapport.messageErreur);
                    printf("\n\n-----------------solde-01--------------------\n\n");
                    printf("code banque : %s\n",fichier->blocs[k].ancienSolde.codeBanque);
                    printf("code guichet : %s\n",fichier->blocs[k].ancienSolde.codeGuichet);
                    printf("Numero de compte : %s\n",fichier->blocs[k].ancienSolde.numeroCompte);
                    printf("Date : %d - %d - %d\n",fichier->blocs[k].ancienSolde.date.jour,
                        fichier->blocs[k].ancienSolde.date.mois,fichier->blocs[k].ancienSolde.date.annee);
                    printf("titulaire : %s\n",fichier->blocs[k].ancienSolde.titulaire);
                    afficherMontant(fichier->blocs[k].ancienSolde.solde);
                    printf("devise : %s\n",fichier->blocs[k].ancienSolde.devise);
                    printf("\n\n-----------------operation-04 & 05--------------------\n\n");
                        for (int i = 0; i < fichier->blocs[k].nbOperations; i++) {
                            printf("===========operation %d=============\n",i+1);
                            printf("numero de compte : %s\n",fichier->blocs[k].operations[i].numeroCompte);
                            printf("Code operation : %s\n",fichier->blocs[k].operations[i].codeOperation);
                            printf("Date operation : %d - %d - %d\n",fichier->blocs[k].operations[i].dateOperation.jour,
                                fichier->blocs[k].operations[i].dateOperation.mois,fichier->blocs[k].operations[i].dateOperation.annee);
                            printf("Date valeur : %d - %d - %d\n",fichier->blocs[k].operations[i].dateValeur.jour,
                                fichier->blocs[k].operations[i].dateValeur.mois,fichier->blocs[k].operations[i].dateValeur.annee);
                            printf("Libelle : %s\n",fichier->blocs[k].operations[i].libelle);
                            afficherMontant(fichier->blocs[k].operations[i].montant);
                            printf("reference : %s\n",fichier->blocs[k].operations[i].reference);
                            for (int j = 0;j<fichier->blocs[k].operations[i].nbComplements%6;++j)
                                printf("complement : %s\n",fichier->blocs[k].operations[i].complements[j]);
                        }
                    printf("\n\n-----------------cloture-07--------------------\n\n");
                    printf("code banque : %s\n",fichier->blocs[k].nouveauSolde.codeBanque);
                    printf("code guichet : %s\n",fichier->blocs[k].nouveauSolde.codeGuichet);
                    printf("Numero de compte : %s\n",fichier->blocs[k].nouveauSolde.numeroCompte);
                    printf("Date : %d - %d - %d\n",fichier->blocs[k].nouveauSolde.date.jour,
                        fichier->blocs[k].nouveauSolde.date.mois,fichier->blocs[k].nouveauSolde.date.annee);
                    afficherMontant(fichier->blocs[k].nouveauSolde.solde);
                    printf("devise : %s\n",fichier->blocs[k].nouveauSolde.devise);
                }
            }
    }
    libererFichier(fichier);
    return 0;
}


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