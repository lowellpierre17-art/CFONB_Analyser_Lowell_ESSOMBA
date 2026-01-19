#ifndef CFONB_PARSER_H_INCLUDED
#define CFONB_PARSER_H_INCLUDED
#include "cfonb_types.h"
//Parse un argument et consevre les propriétés de lq commande saisie
int parserArguments(int argc, char *argv[], Arguments *args);
// Parse une ligne selon son type
RecordType detecterTypeLigne(const char* ligne);
// Parse un enregistrement 01 ou 07
int parseInfoCompte(const char* ligne, InfoCompte* info);
// Parse un enregistrement 04
int parseOperation(const char* ligne, Operation* op);
// Parse un enregistrement 05 et l'ajoute à l'opération courante
int parseComplement(const char* ligne, Operation* op);
// Charge un fichier complet
FichierCFONB* chargerFichier(const char* nomFichier);
// Libère la mémoire
void libererFichier(FichierCFONB* fichier);
// Parse les blocs dans un fichier de 01 jusqu'a 07
int parseBloc(FILE*f,BlocCompte*compte);
//Affiche le chargement du fichier
void chargerficher(FichierCFONB*);
#endif
