#ifndef CFONB_UTILS_H_INCLUDED
#define CFONB_UTILS_H_INCLUDED
#include "cfonb_types.h"

void afficherUsage(const char *nomProgramme);
void afficherArguments(Arguments *args);
// Conversion du caractère signé en montant
Montant decoderMontant(const char* montantStr, int nbDecimales);
// Parsing d'une date JJMMAA
DateCFONB parseDate(const char* dateStr);
// Extraction d'une sous-chaîne (positions CFONB sont en base 1)
void extraireChamp(const char* ligne, int debut, int fin, char* dest);
// Affichage formaté d'un montant
void afficherMontant(Montant m);
// Comparaison de dates
int comparerDates(DateCFONB d1, DateCFONB d2);
// Initialisation d'un fichier CFONB
FichierCFONB* creerFichier(const char* nom);
// Ajouter un bloc (avec réallocation si nécessaire)
int ajouterBloc(FichierCFONB* fichier, BlocCompte bloc);
//Initialisation d'un bloc
BlocCompte* creerBloc();
//Ajouter une operation (avec reallocation si necessaire)
int ajouterOperation(BlocCompte*bloc, Operation operation);
#endif
