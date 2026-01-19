#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cfonb_parser.h"
#include <stdbool.h>
#include "cfonb_types.h"
#include "cfonb_utils.h"

/*Si la fonction retourne 0 ou -1, alors il y a une erreur et il faudra appeler afficherusage*/
int parserArguments(int argc, char *argv[], Arguments *args) {
    // Initialiser avec des valeurs par défaut
    memset(args, 0, sizeof(Arguments));
    args->montantMin = -1;  // -1 = pas de filtre
    // Vérifier le minimum d'arguments
    if (argc < 2) {
        return -1;
    }
    // Gérer --help en premier
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        return 0;  // Code spécial pour afficher l'aide
    }
    // Commande obligatoire
    strncpy(args->fichier, argv[1], 255);
    // Fichier obligatoire (sauf pour help)
    if (argc < 3) {
        printf("Erreur : fichier manquant\n");
        return -1;
    }
    strncpy(args->commande, argv[2], 19);
    // Parser les options
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "--compte") == 0 && i + 1 < argc) {
            strncpy(args->numeroCompte, argv[++i], 11);
        }
        else if ((strcmp(argv[i], "--montant-min") == 0 || strcmp(argv[i], "-m") == 0) && i + 1 < argc) {
            args->montantMin = atol(argv[++i]);
        }
        else if (strcmp(argv[i], "--date") == 0 && i + 1 < argc) {
            strncpy(args->date, argv[++i], 6);
        }
        else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            args->verbose = 1;
        }
        else if (argv[i][0] == '-') {
            printf("Erreur : option inconnue '%s'\n", argv[i]);
            return -1;
        }
    }
    return 1;  // Succès
}


RecordType detecterTypeLigne(const char* ligne) {
    switch (ligne[1]) {
        case '1': return RECORD_OLD_BALANCE;
        case '4': return RECORD_OPERATION;
        case '5': return RECORD_COMPLEMENT;
    }
	return RECORD_NEW_BALANCE;
}

int parseInfoCompte(const char* ligne, InfoCompte* info) {
    if (ligne==NULL)
        return 1;
    extraireChamp(ligne,3,7,info->codeBanque);
    extraireChamp(ligne,12,16,info->codeGuichet);
    extraireChamp(ligne,17,19,info->devise);
    char decimalStr[2];
    extraireChamp(ligne,20,20,decimalStr);
    info->nbDecimales = atoi(decimalStr);
    extraireChamp(ligne,22,32,info->numeroCompte);
    char datestr[7];
    extraireChamp(ligne,35,40,datestr);
    info->date = parseDate(datestr);
    extraireChamp(ligne,41,90,info->titulaire);
    char prix[15];
    extraireChamp(ligne,91,104,prix);
    info->solde = decoderMontant(prix,info->nbDecimales);
    return 0;

}

int parseOperation(const char* ligne, Operation* op) {
    if (ligne==NULL)
        return 1;
    extraireChamp(ligne,22,32,op->numeroCompte);
    extraireChamp(ligne,33,34,op->codeOperation);
    char Date[7];
    extraireChamp(ligne,35,40,Date);
    op->dateOperation = parseDate(Date);
    extraireChamp(ligne,43,48,Date);
    op->dateValeur = parseDate(Date);
    extraireChamp(ligne,49,80,op->libelle);
    char montantstr[14];
    extraireChamp(ligne,92,104,montantstr);
    char decimalstr[2];
    extraireChamp(ligne,20,20,decimalstr);
    op->montant = decoderMontant(montantstr,atoi(decimalstr));
    extraireChamp(ligne,105,120,op->reference);
    op->nbComplements = 0;
    return 0;
}

int parseComplement(const char* ligne, Operation* op)
{
    if (ligne==NULL)
        return 1;
    extraireChamp(ligne,49,118,op->complements[op->nbComplements]);
	op->nbComplements +=1;
    return 0;
}


int parseBloc(FILE*f,BlocCompte*compte) {
    char ligne[120];
    fgets(ligne,120, f);
    if (parseInfoCompte(ligne,&compte->ancienSolde))
        return 1;

    if(!fgets(ligne,120, f)) return 1;
    while(detecterTypeLigne(ligne)!=RECORD_NEW_BALANCE) {
        Operation op;
        memset(&op, 0, sizeof(op));
        if (parseOperation(ligne,&op)!=0)
            return 1;
        if(!fgets(ligne, 120, f)) return 1;
        while (detecterTypeLigne(ligne)==RECORD_COMPLEMENT) {
            if (parseComplement(ligne,&op)!=0)
                    return 1;
            if(!fgets(ligne, 120, f)) return 1;
        }
    	if (compte->nbOperations >= compte->capaciteOperations) {
        compte->capaciteOperations *= 2;
        Operation* nouveau = realloc(compte->operations,compte->capaciteOperations * sizeof(Operation));
        if (!nouveau) return 1;
        compte->operations = nouveau;
    }
    compte->operations[compte->nbOperations++] = op;

    }

    if (parseInfoCompte(ligne,&compte->nouveauSolde)!=0)
            return 1;
    return 0;
}


FichierCFONB* chargerFichier(const char* nomFichier) {
    FILE* f = fopen(nomFichier,"r");
    if (!f) return NULL;
    bool valid = true;
    FichierCFONB* fichier = creerFichier(nomFichier);
    while (valid) {

        BlocCompte*bloc = nouveauBloc();

        if (bloc!=NULL) {
            if (parseBloc(f,bloc)==0 && ajouterBloc(fichier,*bloc)!=0)
               valid = false;
            else if (parseBloc(f,bloc)!=0)
            	valid = false;
			else
				valid = true;
        }
        free(bloc);
        bloc = NULL;
    }
    fclose(f);
    return fichier;
}
// Libère la mémoire
void libererFichier(FichierCFONB* fichier) {
    free(fichier);
    fichier = NULL;
}

void chargerficher(FichierCFONB*fichier) {
    printf("\n=== CHARGEMENT CFONB ===\n\n");
        printf("Fichier : %s\n",fichier->nomFichier);
        int nbEnregistrements = 0, nbOperations = 0, nbComplements = 0;
        for (int i =0;i<fichier->nbBlocs;++i) {
            nbOperations += fichier->blocs[i].nbOperations;
            for (int j =0; j< fichier->blocs[i].nbOperations ;++j)
                nbComplements += fichier->blocs[i].operations[j].nbComplements;
        }
        nbEnregistrements = nbComplements + nbOperations + 2*fichier->nbBlocs;
        printf("Enregistrements lus : %d\n",nbEnregistrements);
        printf("  - Type 01 (ancien solde) : %d\n",fichier->nbBlocs);
        printf("  - Type 04 (operations) : %d\n",nbOperations);
        printf("  - Type 05 (complements) : %d\n",nbComplements);
        printf("  - Type 07 (nouveau solde) : %d\n",fichier->nbBlocs);
        printf("Blocs de comptes détectés: %d\n",fichier->nbBlocs);
        printf("Chargement réussi.\n");
}