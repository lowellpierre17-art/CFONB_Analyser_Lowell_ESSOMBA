#ifndef CFONB_VALIDATOR_H_INCLUDED
#define CFONB_VALIDATOR_H_INCLUDED
#include "cfonb_types.h"

// Valide la structure d'un bloc (séquence 01-04-07)
RapportValidation validerStructureBloc(BlocCompte* bloc);
// Vérifie la cohérence du numéro de compte
RapportValidation validerCoherenceCompte(BlocCompte* bloc);
// Recalcule et vérifie le solde
RapportValidation validerSolde(BlocCompte* bloc);
// Validation complète d'un fichier
RapportValidation* validerFichier(FichierCFONB* fichier, int* nbRapports);
//Afficher le rapport complet éffectué sur un fichier CFONB
void afficherValiderFichier(char* srcFichier);

#endif
