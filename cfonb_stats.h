#ifndef CFONB_STATS_H_INCLUDED
#define CFONB_STATS_H_INCLUDED
#include "cfonb_types.h"

// Calcule les stats d'un bloc
StatsCompte calculerStatsBloc(BlocCompte* bloc);
// Affiche les stats de tous les comptes
void afficherStats(char*);
// Recherche des opérations selon critères
Operation** rechercherOperations(FichierCFONB* fichier,const char* numeroCompte,long montantMin,DateCFONB* date,int* nbResultats);
//Affiche les résultats obtenus de la recherche contenus dans le pointeur d'opérations
void afficherRechercherOperations(char* srcFichier,Arguments* args);
//Structure et organise la gestion des recherches afin d'afficher les résultats correspondants aux parametres
void afficheResultats(Operation**operation, char* option, char* valeur, int nbOperations);
#endif
