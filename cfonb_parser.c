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

/*
 *La fonction analyse les deux premiers caractères de la ligne et restitue un type selon un certain enregistrement
 *Si la ligne ne débute pas avec un '0', alors elle renvoie un type inconnu de même si le caractère suivant ne correspond
 * pas à ceux définis dans le format CFonb120
 */
RecordType detecterTypeLigne(const char* ligne) {
    RecordType detector;
    if (ligne[0]=='0') {
         switch (ligne[1]) {
            case '1': detector = RECORD_OLD_BALANCE; break;
            case '4': detector = RECORD_OPERATION; break;
            case '5': detector = RECORD_COMPLEMENT; break;
            case '7': detector = RECORD_NEW_BALANCE; break;
            default: detector = RECORD_UNKOWN; break;
        }
    }
    else
        detector = RECORD_UNKOWN;
    return detector;
}
// Parse un enregistrement 01 ou 07
/*
 * La fonction retourne 1 si la ligne du fichier passé en argument est vide
 * Retourne 0 si le fichier est lisible et stocke les données utiles dans la structure adaptée.
 */
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
    char montantstr[15];
    extraireChamp(ligne,91,104,montantstr);
    info->solde = decoderMontant(montantstr,info->nbDecimales);
    return 0;

}
// Parse un enregistrement 04
/*
 * La fonction retourne 1 si la ligne du fichier passé en argument est vide
 * Retourne 0 si le fichier est lisible et stocke les données utiles dans la structure adaptée (opération).
 */
int parseOperation(const char* ligne, Operation* op) {
    if (ligne==NULL)
        return 1;
    extraireChamp(ligne,22,32,op->numeroCompte);
    extraireChamp(ligne,33,34,op->codeOperation);
    char datestr[7];
    extraireChamp(ligne,35,40,datestr);
    op->dateOperation = parseDate(datestr);
    extraireChamp(ligne,43,48,datestr);
    op->dateValeur = parseDate(datestr);
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

/*Parse un enregistrement 05 et l'ajoute à l'opération courante
 *Pour chaque complément, la fonction ajoute le libellé de ces différents compléments au tableau de compléments figurant
 * dans l'opération concernée
 */
int parseComplement(const char* ligne, Operation* op)
{
    if (ligne==NULL)
        return 1;
    extraireChamp(ligne,49,118,op->complements[op->nbComplements++]);
    return 0;
}

/*
 *La fonction lit une ligne et la considère comme solde initial ensuite,
 *vérifie que les lignes suivantes ne correspondent pas à un nouveau solde et les enregistre comme des opérations et
 *comme des compléments. Sinon, elle lit la ligne suivante comme nouveau solde.
 */
int parseBloc(FILE*f,BlocCompte*compte) {
    char line[size_file_cfonb];
    if (fgets(line, size_file_cfonb, f)) {
        if (parseInfoCompte(line,&compte->ancienSolde))
            return 1;
    }

    fgets(line, size_file_cfonb, f);
    while(detecterTypeLigne(line)!=RECORD_NEW_BALANCE) {
        Operation operation = {0};
        if (parseOperation(line,&operation)!=0)
            return 1;
        while (fgets(line, size_file_cfonb, f) && detecterTypeLigne(line)==RECORD_COMPLEMENT) {
            if (parseComplement(line,&operation)!=0)
                    return 1;
        }
        if (ajouterOperation(compte,operation)!=0)
            return 1;
    }

    if (parseInfoCompte(line,&compte->nouveauSolde)!=0)
            return 1;
    return 0;
}

/*
 *En utilisant le mode restitution de '0' impliquant que le résultat est valide, tant que les résultats des différentes
 *opérations de parsing ne sont pas nuls, alors la constitution du fichier complet se poursuit par ajouts des
 *differents blocs
 */
FichierCFONB* chargerFichier(const char* nomFichier) {
    FILE* f = fopen(nomFichier,"r");
    if (!f) return NULL;
    bool parsingBlocs = true;
    FichierCFONB* fichier = creerFichier(nomFichier);
    while (parsingBlocs) {
        BlocCompte*bloc = creerBloc();
        if (bloc!=NULL) {
            if (parseBloc(f,bloc)==0) {
                if (ajouterBloc(fichier,*bloc)!=0)
                    parsingBlocs = false;
            }
            else
                parsingBlocs = false;
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

/*
 *Cette fonction permet restituer l'analyse du fichier chargé en fonction du nom (la source) du fichier
 *Elle se sert de la constituion complete du fichier afin de fournir les résultats
 */
void chargerEtAfficher(char* srcFichier) {
    printf("\n=== CHARGEMENT CFONB ===\n\n");
    FichierCFONB*fichier = chargerFichier(srcFichier);
    if (!fichier) printf("Echec de chargement\n");
    else {
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
    libererFichier(fichier);
}