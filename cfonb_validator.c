#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfonb_validator.h"
#include <stdbool.h>
#include "cfonb_parser.h"
#include "cfonb_types.h"

// Valide la structure d'un bloc (séquence 01-04-07)
RapportValidation validerStructureBloc(BlocCompte* bloc) {
    RapportValidation rapport = { VALID_OK, 0, "Structure valide  Ok" };
    // Vérification de la validité du format du numéro de compte et du code banque
    // ainsi que la présence des deux premiers caractères du nom du titulaire du compte
    char verifierEspace[3];
    strncpy(verifierEspace, bloc->ancienSolde.titulaire, 2);verifierEspace[2] = '\0';
    if (strlen(bloc->ancienSolde.numeroCompte) !=11 || strlen(bloc->ancienSolde.codeBanque) != 5
        || strcmp(verifierEspace,"  ")==0) {
        rapport.resultat = VALID_ERR_SEQUENCE;
        strcpy(rapport.messageErreur, "Ancien solde invalide ou absent!!");
        return rapport;
        }
    //Vérifie la validité du format du numéro de compte, du code banque et
    //s'assure de l'égalité des codes banque de l'ancien et le nouveau solde
    if (strcmp(bloc->nouveauSolde.codeBanque,bloc->ancienSolde.codeBanque)!=0 || strlen(bloc->nouveauSolde.codeBanque) != 5
       || strlen(bloc->nouveauSolde.numeroCompte) !=11)
        {
        rapport.resultat = VALID_ERR_SEQUENCE;
        strcpy(rapport.messageErreur, "Nouveau solde invalide ou absent !!");
        return rapport;
        }
    {
        //Pour chaque opération, vérifie la validité de la date valeur ainsi qu'un d'un montant d'une opération non-nul
        bool good = true;
        while (rapport.ligneErreur < bloc->nbOperations && good) {
            if (bloc->operations[rapport.ligneErreur].dateValeur.jour==0 ||
                bloc->operations[rapport.ligneErreur].montant.centimes==0)
                good = false;
            rapport.ligneErreur++;
        }
        if (!good) {
            rapport.resultat = VALID_ERR_SEQUENCE;
            sprintf(rapport.messageErreur,"Operation %d invalide !!", rapport.ligneErreur);
            return rapport;
        }
    }
    return rapport;
}

// Vérifie la cohérence du numéro de compte
RapportValidation validerCoherenceCompte(BlocCompte* bloc) {
    RapportValidation rapport = { VALID_OK, 0, "Cohérence compte  Ok" };
    //Etant donné qu'une comparaison de numéro de compte ne puisse être faite, on s'assure juste de valider le format
    //du numéro de compte de l'ancien solde
    if (strlen(bloc->ancienSolde.numeroCompte)!=11) {
        rapport.resultat = VALID_ERR_COMPTE;
        strcpy(rapport.messageErreur, "Numero de compte d'ouverture invalide");
        return rapport;
    }
    //Vérification de la correspondance du num'ro de compte avec celui de l'ancien solde
    if (strcmp(bloc->nouveauSolde.numeroCompte,bloc->ancienSolde.numeroCompte)!=0) {
        rapport.resultat = VALID_ERR_COMPTE;
        strcpy(rapport.messageErreur, "Numero de compte de cloture incoherent");
        return rapport;
    }
    //Pour chacun des opérations, on effectue une vérification de l'égalité entre chacune des opérations et l'ancien solde
    while (strcmp(bloc->operations[rapport.ligneErreur].numeroCompte,bloc->ancienSolde.numeroCompte)==0 &&
        rapport.ligneErreur<bloc->nbOperations)
        rapport.ligneErreur++;
    if (rapport.ligneErreur<bloc->nbOperations) {
       rapport.resultat = VALID_ERR_COMPTE;
        sprintf(rapport.messageErreur,"Numero de compte de l'operation %d invalide",rapport.ligneErreur+1);
        return rapport;
    }
    return rapport;
}

// Recalcule et vérifie le solde
RapportValidation validerSolde(BlocCompte* bloc) {
    RapportValidation rapport = { VALID_OK, 0, "Solde vérifié     OK" };
    // Partir du solde initial
    unsigned long soldeCalcule = bloc->ancienSolde.solde.centimes;
    if (bloc->ancienSolde.solde.sens == SENS_DEBIT) {
        soldeCalcule = -soldeCalcule;
    }
    // Appliquer chaque opération
    for (int i = 0; i < bloc->nbOperations; i++) {
        unsigned long montant = bloc->operations[i].montant.centimes;
        if (bloc->operations[i].montant.sens == SENS_CREDIT) {
            soldeCalcule += montant;
        } else {
            soldeCalcule -= montant;
        }
    }
    // Comparer avec le solde déclaré
    unsigned long soldeDeclare = bloc->nouveauSolde.solde.centimes;
    if (bloc->nouveauSolde.solde.sens == SENS_DEBIT) {
        soldeDeclare = -soldeDeclare;
    }
    if (soldeCalcule != soldeDeclare) {
        rapport.resultat = VALID_ERR_SOLDE;
        sprintf(rapport.messageErreur,
                "Solde incohérent: calculé=%ld, déclaré=%ld (diff=%ld centimes)",
                soldeCalcule, soldeDeclare, soldeCalcule - soldeDeclare);
    }
    return rapport;
}


// Construit un tableau de rapports pour chaque bloc d'un fichier CFonb sachant que pour chaque bloc l'on obtient 03 rapports
RapportValidation* validerFichier(FichierCFONB* fichier, int* nbRapports){
    if (!fichier || !nbRapports)
        return NULL;
    // Allocation maximale possible : 3 erreurs par bloc
    RapportValidation* rapports = malloc(sizeof(RapportValidation) * (3*fichier->nbBlocs));
    if (!rapports)
        return NULL;
    int index = 0;
    for (int i = 0; i < fichier->nbBlocs; i++) {
        int valide = 0;
        RapportValidation r = validerStructureBloc(&fichier->blocs[i]);
        rapports[index++] = r;
        if (r.resultat == VALID_OK)
            valide++;
        r = validerSolde(&fichier->blocs[i]);
        rapports[index++] = r;
        if (r.resultat == VALID_OK)
            valide++;
        r = validerCoherenceCompte(&fichier->blocs[i]);
        rapports[index++] = r;
        if (r.resultat == VALID_OK)
            valide++;
        if (valide==3)
            *nbRapports+=1;
    }
    return rapports;
}
//Affiche le bilan de l'évaluation du fichier CFOnb
void afficherValiderFichier(char* srcFichier) {
    printf("\n=== VALIDATION CFONB === \n\n");
    FichierCFONB*fichier= chargerFichier(srcFichier);
    if (!fichier) printf("Fichier introuvable");
    else {
        int nbRapports=0;
        RapportValidation* rapport = validerFichier(fichier, &nbRapports);
        printf("Fichier : %s\n",fichier->nomFichier);
        for (int i = 0; i < fichier->nbBlocs; i++) {
            printf("Bloc %d - Compte %s\n",i+1,fichier->blocs[i].ancienSolde.numeroCompte);
            for (int j = 0; j < 3; j++)
                printf("%s\n",rapport[3*i+j].messageErreur);
            printf("\n");
        }
        printf("Résultat: %d/%d blocs valides\n",nbRapports,fichier->nbBlocs);
        free(rapport);
        rapport = NULL;
    }
    libererFichier(fichier);
}