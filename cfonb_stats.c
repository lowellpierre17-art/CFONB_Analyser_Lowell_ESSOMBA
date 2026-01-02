#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfonb_stats.h"
#include "cfonb_parser.h"
#include "cfonb_types.h"
#include "cfonb_utils.h"

/*
 *Cette fonction récupère un bloc, parcourt ses différents parametres, filtre et construit les parametres d'une
 *structure de statistiques
 */
StatsCompte calculerStatsBloc(BlocCompte* bloc) {
    StatsCompte stat = {0};
    strcpy(stat.numeroCompte,bloc->ancienSolde.numeroCompte);
    strcpy(stat.titulaire,bloc->ancienSolde.titulaire);
    stat.soldeInitial = bloc->ancienSolde.solde;
    stat.soldeFinal = bloc->nouveauSolde.solde;
    stat.nbCredits = 0;
    stat.nbDebits = 0;
    stat.totalCredits = 0;
    stat.totalDebits = 0;
    for (int i = 0; i < bloc->nbOperations; i++) {
        if (bloc->operations[i].montant.sens == SENS_DEBIT) {
            stat.nbDebits++;
            stat.totalDebits += (long)bloc->operations[i].montant.centimes;
        }
        else {
            stat.nbCredits++;
            stat.totalCredits += (long)bloc->operations[i].montant.centimes;
        }
    }

    stat.variation = stat.totalCredits - stat.totalDebits;
    return stat;
}
// Affiche les stats de tous les comptes
void afficherStats(char* srcFichier) {
    printf("=== STATISTIQUES CFONB ===\n");
    FichierCFONB* fichier = chargerFichier(srcFichier);
    if (!fichier) printf("Echec de chargement\n");
    else {
        for (int j = 0; j < fichier->nbBlocs; j++) {
            StatsCompte stat = calculerStatsBloc(&fichier->blocs[j]);
            printf("Compte: %s - %s\n",stat.numeroCompte,stat.titulaire);
            printf("Solde initial : ");
            afficherMontant(stat.soldeInitial);
            printf("Solde final : ");
            afficherMontant(stat.soldeFinal);
            printf("Operations : %d debits/ %d credits\n",stat.nbDebits,stat.nbCredits);
            printf("Total debits : %.2f €\n", (float)stat.totalDebits/100);
            printf("Total credits : %.2f €\n", (float)stat.totalCredits/100);
            printf("Variation : %.2f\n\n",(float)stat.variation/100);

        }
    }
}

 //Recherche des enregistrements en fonction des parametres passés
Operation** rechercherOperations(FichierCFONB* fichier,const char* numeroCompte,
long montantMin,DateCFONB* date,int* nbResultats){
    Operation** operation = NULL;
/*Effectue une recherche en fonction des correspondances des numéros de compte.
 *Elle prend comme reference le numero de compte de l'ancien solde (étant donné qu'il est commun à toutes les opérations)
 *Et recence toute les opérations de ce bloc
 */
    if (numeroCompte != NULL) {
        for (int k =0; k<fichier->nbBlocs;++k) {
            if (strcmp(fichier->blocs[k].ancienSolde.numeroCompte,numeroCompte) == 0) {
                Operation ** nouveau = realloc(operation, sizeof(Operation*)*(*nbResultats + fichier->blocs[k].nbOperations));
                if (!nouveau) return NULL;
                operation = nouveau;
                for (int j =*nbResultats; j<*nbResultats+fichier->blocs[k].nbOperations; ++j) {
                    operation[j] = &fichier->blocs[k].operations[j-*nbResultats];
                }
                *nbResultats+=fichier->blocs[k].nbOperations;
            }
        }
    }
/*
 *Parcourt les opérations de chaque bloc et vérifie l'ordre de supériorité ou d'égalité avec chacun des montants
 * correspondant à ces opérations et enregistre ceux qui y sont supérieurs ou égaux
 */
    else if (montantMin > 0) {
        for (int k =0; k<fichier->nbBlocs;++k) {
            for (int j = 0; j<fichier->blocs[k].nbOperations; ++j) {
                if (montantMin <= (long)fichier->blocs[k].operations[j].montant.centimes) {
                    Operation** nouveau = realloc(operation,sizeof(Operation*)*(*nbResultats+1));
                    if (!nouveau) return NULL;
                    operation = nouveau;
                    operation[*nbResultats] = &fichier->blocs[k].operations[j];
                    *nbResultats+=1;
                }
            }
        }
    }
/*
 *Pour chacune des ioérations provenant des blocs distincts, vérifie la concordance exacte avec les date opération et
 *les enregistre si ces dates sont égales
 */
    else if (date!=NULL) {
        for (int k =0; k<fichier->nbBlocs;++k) {
            for (int j = 0; j<fichier->blocs[k].nbOperations; ++j) {
                if (comparerDates(fichier->blocs[k].operations[j].dateOperation, *date)==0) {
                    Operation** nouveau = realloc(operation,sizeof(Operation*)*(*nbResultats+1));
                    if (!nouveau) return NULL;
                    operation = nouveau;
                    operation[*nbResultats] = &fichier->blocs[k].operations[j];
                    *nbResultats+=1;
                }
            }
        }
    }
//Si aucun résultat trouvé, alors on retourne un pointeur nul
    if (*nbResultats ==0) {
        free(operation);
        return NULL;
    }
    return operation;
}
//Affiche les resultats des opérations en prenant un parametre le tableau d'opération déja constitué
// et les critères sur lesquels se sont basées les recherches
void afficheResultats(Operation**operation, char* option, char* valeur, int nbOperations) {
        printf("Critere : %s = %s\n",option,valeur);
    if (operation == NULL || nbOperations == 0) {
        printf("Aucun resultat trouve.\n");
    }
    else {
        printf("Resultats : %d operation(s) trouvees(s)\n",nbOperations);
        printf("Date          | Montant          | Libelle\n");
        printf("------------------------------------------------------------------\n");
        for (int i =0; i<nbOperations; i++) {
            printf("%d-%d-%d       |",operation[i]->dateOperation.jour,operation[i]->dateOperation.mois,operation[i]->dateOperation.annee);
            float montant = (float)operation[i]->montant.centimes/100;
            printf("%10.2f €      |",operation[i]->montant.sens==SENS_CREDIT ? montant: (-1*montant));
            printf("%s\n",operation[i]->libelle);
        }
    }
}


//Organise la recherche et l'affichage des résultats trouvés en fonction de la validité des données passées en argument
void afficherRechercherOperations(char* srcFichier,Arguments* args) {
    printf("\n=========RECHERCHE D'OPERATIONS==========\n\n");
    FichierCFONB* fichier = chargerFichier(srcFichier);
    if (!fichier || !args) printf("Fichier introuvable\n");
    else {
        int nbResultats = 0;
        if (strlen(args->numeroCompte) == 11) {
            Operation** operation = rechercherOperations(fichier,args->numeroCompte,0,NULL,&nbResultats);
            afficheResultats(operation,"Compte",args->numeroCompte,nbResultats);
            free(operation); operation = NULL;
        }
        if (strlen(args->date) == 6) {
            DateCFONB date = parseDate(args->date);
            Operation** operation = rechercherOperations(fichier,NULL,0,&date,&nbResultats);
            afficheResultats(operation,"Date",args->date,nbResultats);
            free(operation); operation = NULL;
        }
        if (args->montantMin>=0) {
            char montantStr[10];
            sprintf(montantStr,"%ld",args->montantMin);
            Operation** operation = rechercherOperations(fichier,NULL,args->montantMin,NULL,&nbResultats);
            afficheResultats(operation,"Montant",montantStr,nbResultats);
            free(operation); operation = NULL;
        }
    }
}