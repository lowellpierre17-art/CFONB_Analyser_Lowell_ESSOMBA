# TP4 - Algorithmique Fondamentaux
## Analyseur de Relevés Bancaires CFONB120

<p>
  <img
    src="https://upload.wikimedia.org/wikipedia/commons/8/86/LOGO-ensim-couleur-HD.png"
    width="550"
    alt="Logo ENSIM">
</p>

**1er Cycle Ingénieur | Niveau Bac+3**

**Module : Algorithmique Fondamentaux**

**TP Final - Évaluation notée**

---

## Table des matières

1. [Contexte professionnel](#1-contexte-professionnel)
2. [Objectifs pédagogiques](#2-objectifs-pédagogiques)
3. [Les arguments en ligne de commande (argc/argv)](#3-les-arguments-en-ligne-de-commande-argcargv)
4. [Le format CFONB120](#4-le-format-cfonb120)
5. [Spécifications fonctionnelles](#5-spécifications-fonctionnelles)
6. [Architecture imposée](#6-architecture-imposée)
7. [Types et structures de données](#7-types-et-structures-de-données)
8. [Travail demandé](#8-travail-demandé)
9. [Contraintes techniques](#9-contraintes-techniques)
10. [Conseils d'implémentation](#10-conseils-dimplémentation)
11. [Consignes de rendu GitHub](#11-consignes-de-rendu-github)
12. [Barème de notation](#12-barème-de-notation)
13. [Annexe A - Tables de codes](#annexe-a---tables-de-codes)
14. [Annexe B - Exemples de sorties attendues](#annexe-b---exemples-de-sorties-attendues)

---

## 1. Contexte professionnel

### 1.1 Votre mission

Vous venez d'être embauché chez **KPMG Paris** en tant que **Consultant Junior en Flux Bancaires**. Dès votre première semaine, votre manager vous confie une mission stratégique pour un client du cabinet : **JM Comptabilité**, un cabinet comptable au Mans en pleine expansion.

JM Comptabilité reçoit quotidiennement des centaines de relevés de comptes au format **CFONB120** (format standardisé français pour les échanges bancaires) de la part de leur prestataire **SoBank**. 

So bank est une solution / plateforme de collecte et traitement automatisé des relevés bancaires destinée surtout aux cabinets d’expertise comptable, PME, TPE ou indépendants. Ils font l’intermédiaire entre les établissements bancaires et leurs clients.

Actuellement, ces fichiers sont traités manuellement par les équipes comptables, ce qui génère des erreurs et des retards considérables.

### 1.2 Le besoin client

Le directeur financier de JM Comptabilité souhaite automatiser le traitement de ces relevés. Il vous demande de développer un **outil en ligne de commande** capable de :

- Lire et parser les fichiers CFONB120
- Vérifier l'intégrité et la cohérence des données
- Extraire des statistiques et indicateurs clés

### 1.3 Enjeux réels

> *« Dans le secteur bancaire, une erreur de parsing peut coûter des millions. Votre code sera audité par nos équipes conformité. La rigueur n'est pas une option. »*
> - Votre manager, KPMG

Ce TP simule une situation professionnelle réelle. Le format CFONB120 est **réellement utilisé** par les banques françaises depuis 1992. Les fichiers fournis sont représentatifs de vrais relevés bancaires (données anonymisées).

---

## 2. Objectifs pédagogiques


À l'issue de ce TP, vous démontrerez votre capacité à :

| Compétence | Description |
|------------|-------------|
| **Parsing de fichiers** | Lire et interpréter des fichiers texte à format fixe |
| **Structures de données** | Concevoir des structures adaptées à un domaine métier |
| **Algorithmique** | Implémenter des algorithmes de validation et d'agrégation |
| **Gestion mémoire** | Utiliser l'allocation dynamique de manière sécurisée |
| **Modularité** | Organiser un projet en modules cohérents |
| **Robustesse** | Gérer les cas d'erreur et les données malformées |

Ce TP exige une **réflexion ingénieur** : vous devez analyser une spécification technique, concevoir une architecture robuste, et produire un code de qualité professionnelle.

---

## 3. Les arguments en ligne de commande (argc/argv)

### 3.1 Qu'est-ce qu'un programme en ligne de commande ?

Jusqu'à présent, vos programmes interagissaient avec l'utilisateur via `scanf()` pendant l'exécution. Les **programmes en ligne de commande** fonctionnent différemment : ils reçoivent toutes leurs instructions **au moment du lancement**.

**Exemple concret** : Quand vous tapez dans un terminal :

```bash
ls -la /home/etudiant
```

Vous exécutez le programme `ls` avec :
- Une **option** : `-la` (affichage détaillé + fichiers cachés)
- Un **argument** : `/home/etudiant` (le dossier à lister)

Le programme `ls` n'a pas besoin de vous demander "Quel dossier voulez-vous lister ?" - il le sait déjà grâce aux arguments.

### 3.2 Comment ça fonctionne en C ?

La fonction `main()` peut recevoir deux paramètres spéciaux :

```c
int main(int argc, char *argv[]) {
    // argc = nombre d'arguments (argument count)
    // argv = tableau des arguments (argument values)
}
```

**Exemple** : Si vous lancez `./monprog fichier.txt -v`

| Variable | Valeur |
|----------|--------|
| `argc` | 3 |
| `argv[0]` | `"./monprog"` (nom du programme) |
| `argv[1]` | `"fichier.txt"` |
| `argv[2]` | `"-v"` |

> **Important** : `argv[0]` contient toujours le nom du programme. Les vrais arguments commencent à `argv[1]`.

### 3.3 Exemple simple : afficher les arguments

```c
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Nombre d'arguments : %d\n", argc);

    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = \"%s\"\n", i, argv[i]);
    }

    return 0;
}
```

**Compilation et test** :
```bash
$ gcc -o test_args test_args.c
$ ./test_args bonjour 42 fichier.txt
Nombre d'arguments : 4
argv[0] = "./test_args"
argv[1] = "bonjour"
argv[2] = "42"
argv[3] = "fichier.txt"
```

### 3.4 Vérifier le nombre d'arguments

Un programme robuste doit **toujours** vérifier qu'il a reçu les bons arguments :

```c
#include <stdio.h>

int main(int argc, char *argv[]) {
    // On attend au moins 2 arguments : le programme + un fichier
    if (argc < 2) {
        printf("Erreur : fichier manquant\n");
        printf("Usage : %s <fichier>\n", argv[0]);
        return 1;  // Code d'erreur
    }

    printf("Fichier à traiter : %s\n", argv[1]);
    return 0;  // Succès
}
```

**Test** :
```bash
$ ./monprog
Erreur : fichier manquant
Usage : ./monprog <fichier>

$ ./monprog data.txt
Fichier à traiter : data.txt
```

### 3.5 Gérer une commande (action à effectuer)

Votre programme aura plusieurs fonctionnalités. L'utilisateur indique quelle action il veut via le **premier argument** :

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage : %s <commande> [options]\n", argv[0]);
        printf("Commandes disponibles :\n");
        printf("  load <fichier>   - Charger un fichier\n");
        printf("  stats <fichier>  - Afficher les statistiques\n");
        printf("  help             - Afficher cette aide\n");
        return 1;
    }

    // Récupérer la commande (premier argument après le nom du programme)
    char *commande = argv[1];

    // Comparer la commande avec strcmp()
    if (strcmp(commande, "help") == 0) {
        printf("Aide du programme...\n");
    }
    else if (strcmp(commande, "load") == 0) {
        // Vérifier qu'on a bien un fichier en plus
        if (argc < 3) {
            printf("Erreur : fichier manquant pour la commande 'load'\n");
            printf("Usage : %s load <fichier>\n", argv[0]);
            return 1;
        }
        char *fichier = argv[2];
        printf("Chargement du fichier : %s\n", fichier);
    }
    else if (strcmp(commande, "stats") == 0) {
        if (argc < 3) {
            printf("Erreur : fichier manquant pour la commande 'stats'\n");
            return 1;
        }
        char *fichier = argv[2];
        printf("Statistiques du fichier : %s\n", fichier);
    }
    else {
        printf("Erreur : commande inconnue '%s'\n", commande);
        return 1;
    }

    return 0;
}
```

### 3.6 Gérer les options (paramètres optionnels)

Les **options** sont des arguments qui modifient le comportement du programme. Par convention :
- Elles commencent par `-` (forme courte) ou `--` (forme longue)
- Elles peuvent avoir une valeur associée

**Exemples d'options** :
```bash
./monprog stats fichier.txt --verbose          # Option sans valeur
./monprog search fichier.txt --compte 12345    # Option avec valeur
./monprog search fichier.txt -m 100            # Forme courte
```

**Implémentation** :

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage : %s search <fichier> [options]\n", argv[0]);
        printf("Options :\n");
        printf("  --compte <num>     Filtrer par numéro de compte\n");
        printf("  --montant-min <n>  Montant minimum\n");
        printf("  --verbose, -v      Mode détaillé\n");
        return 1;
    }

    // Arguments obligatoires
    char *commande = argv[1];
    char *fichier = argv[2];

    // Valeurs par défaut des options
    char *numeroCompte = NULL;    // Pas de filtre par défaut
    int montantMin = 0;           // Pas de minimum
    short verbose = 0;              // Mode silencieux

    // Parcourir les arguments à partir de argv[3]
    for (int i = 3; i < argc; i++) {

        if (strcmp(argv[i], "--compte") == 0) {
            // L'argument suivant est la valeur
            if (i + 1 >= argc) {
                printf("Erreur : --compte nécessite une valeur\n");
                return 1;
            }
            numeroCompte = argv[i + 1];
            i++;  // Sauter la valeur qu'on vient de lire
        }
        else if (strcmp(argv[i], "--montant-min") == 0 || strcmp(argv[i], "-m") == 0) {
            if (i + 1 >= argc) {
                printf("Erreur : --montant-min nécessite une valeur\n");
                return 1;
            }
            montantMin = atoi(argv[i + 1]);  // Convertir en entier
            i++;
        }
        else if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            verbose = 1;  // Option sans valeur = flag booléen
        }
        else {
            printf("Erreur : option inconnue '%s'\n", argv[i]);
            return 1;
        }
    }

    // Afficher ce qu'on a compris
    printf("Commande : %s\n", commande);
    printf("Fichier : %s\n", fichier);
    printf("Filtre compte : %s\n", numeroCompte ? numeroCompte : "(aucun)");
    printf("Montant min : %d\n", montantMin);
    printf("Mode verbose : %s\n", verbose ? "oui" : "non");

    return 0;
}
```

**Tests** :
```bash
$ ./monprog search data.txt
Commande : search
Fichier : data.txt
Filtre compte : (aucun)
Montant min : 0
Mode verbose : non

$ ./monprog search data.txt --compte 02863280452 --verbose
Commande : search
Fichier : data.txt
Filtre compte : 02863280452
Montant min : 0
Mode verbose : oui

$ ./monprog search data.txt -m 500 --compte 12345
Commande : search
Fichier : data.txt
Filtre compte : 12345
Montant min : 500
Mode verbose : non
```

### 3.7 Structure recommandée pour votre projet

Créez une structure pour stocker les arguments parsés :

```c
// Dans cfonb_types.h
typedef struct {
    char commande[20];        // "load", "validate", "stats", "search"
    char fichier[256];        // Chemin du fichier
    char numeroCompte[12];    // Filtre par compte (optionnel)
    long montantMin;          // Filtre montant minimum
    char date[7];             // Filtre par date JJMMAA
    short verbose;            // Mode détaillé
} Arguments;

// Fonction pour parser les arguments
int parserArguments(int argc, char *argv[], Arguments *args);
```

```c
// Dans main.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cfonb_types.h"

void afficherUsage(const char *nomProgramme) {
    printf("Usage : %s <commande> <fichier> [options]\n\n", nomProgramme);
    printf("Commandes :\n");
    printf("  load <fichier>       Charger et afficher un résumé\n");
    printf("  validate <fichier>   Valider la cohérence des données\n");
    printf("  stats <fichier>      Afficher les statistiques\n");
    printf("  search <fichier>     Rechercher des opérations\n\n");
    printf("Options pour 'search' :\n");
    printf("  --compte <num>       Filtrer par numéro de compte\n");
    printf("  --montant-min <n>    Montant minimum en centimes\n");
    printf("  --date <JJMMAA>      Filtrer par date\n\n");
    printf("Options générales :\n");
    printf("  --verbose, -v        Affichage détaillé\n");
    printf("  --help, -h           Afficher cette aide\n");
}

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
    strncpy(args->commande, argv[1], 19);

    // Fichier obligatoire (sauf pour help)
    if (argc < 3) {
        printf("Erreur : fichier manquant\n");
        return -1;
    }
    strncpy(args->fichier, argv[2], 255);

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

int main(int argc, char *argv[]) {
    Arguments args;

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
    }

    return 0;
}
```

### 3.8 Fonctions utiles pour les arguments

| Fonction | Description | Exemple |
|----------|-------------|---------|
| `strcmp(s1, s2)` | Compare deux chaînes (0 si égales) | `if (strcmp(argv[1], "load") == 0)` |
| `strncpy(dest, src, n)` | Copie au maximum n caractères | `strncpy(args.fichier, argv[2], 255)` |
| `atoi(s)` | Convertit une chaîne en `int` | `int n = atoi(argv[3])` |
| `atol(s)` | Convertit une chaîne en `long` | `long m = atol(argv[3])` |
| `atof(s)` | Convertit une chaîne en `double` | `double x = atof(argv[3])` |

### 3.9 Bonnes pratiques

1. **Toujours vérifier `argc`** avant d'accéder à `argv[i]`
2. **Afficher une aide claire** avec `--help` ou en cas d'erreur
3. **Valider les valeurs** (fichier existe ? nombre valide ?)
4. **Utiliser des codes de retour** : 0 = succès, autre = erreur
5. **Gérer les options manquantes** avec des valeurs par défaut

### 3.10 Résumé des commandes de votre programme

```bash
# Aide
./cfonb_analyzer --help

# Charger un fichier
./cfonb_analyzer load data/fichier.txt

# Valider un fichier
./cfonb_analyzer validate data/fichier.txt
./cfonb_analyzer validate data/fichier.txt --verbose

# Statistiques
./cfonb_analyzer stats data/fichier.txt

# Recherche avec filtres
./cfonb_analyzer search data/fichier.txt --compte 02863280452
./cfonb_analyzer search data/fichier.txt --montant-min 10000
./cfonb_analyzer search data/fichier.txt --date 281125
./cfonb_analyzer search data/fichier.txt --compte 02863280452 --montant-min 500 -v
```

---

## 4. Le format CFONB120

### 4.1 Présentation générale

Le format **CFONB120** (Comité Français d'Organisation et de Normalisation Bancaires) est un standard français pour la transmission électronique des relevés de compte. Chaque ligne fait exactement **120 caractères** et représente un enregistrement.

**Document de référence** : Consultez le fichier `CFONB120_specification.pdf` fourni avec ce TP pour les détails complets de la norme.

### 4.2 Structure d'un fichier

Un fichier CFONB120 contient plusieurs **blocs de compte**, chacun composé de :

```
┌─────────────────────────────────────────────────────────────┐
│  Enregistrement 01 - Ancien solde (ouverture)               │
├─────────────────────────────────────────────────────────────┤
│  Enregistrement 04 - Opération 1                            │
│  Enregistrement 05 - Complément (optionnel)                 │
│  Enregistrement 05 - Complément (optionnel)                 │
│  ...                                                        │
│  Enregistrement 04 - Opération N                            │
│  Enregistrement 05 - Complément (optionnel)                 │
├─────────────────────────────────────────────────────────────┤
│  Enregistrement 07 - Nouveau solde (clôture)                │
└─────────────────────────────────────────────────────────────┘
```

### 4.3 Types d'enregistrements

| Code | Type | Description |
|------|------|-------------|
| `01` | Ancien solde | Solde du compte en début de période |
| `04` | Opération | Mouvement (débit ou crédit) |
| `05` | Complément | Informations additionnelles sur l'opération précédente |
| `07` | Nouveau solde | Solde du compte en fin de période |

### 4.4 Structure détaillée des enregistrements

#### Enregistrement 01 - Ancien solde

| Position | Longueur | Contenu          | Description                              |
|----------|----------|------------------|------------------------------------------|
| 1-2      | 2        | `01`             | Code enregistrement                      |
| 3-7      | 5        | `NNNNN`          | Code banque                              |
| 8-11     | 4        | `    `           | Zone réservée                            |
| 12-16    | 5        | `NNNNN`          | Code guichet                             |
| 17-19    | 3        | `EUR`            | Code devise (ISO)                        |
| 20       | 1        | `N`              | Nombre de décimales                      |
| 21       | 1        | ` `              | Zone réservée                            |
| 22-32    | 11       | `NNNNNNNNNNN`    | Numéro de compte                         |
| 33-34    | 2        | `  `             | Zone réservée                            |
| 35-40    | 6        | `JJMMAA`         | Date de l'ancien solde (format français) |
| 41-90    | 50       | `TEXTE`          | Nom du titulaire + libellé compte        |
| 91-104   | 14       | `NNNNNNNNNNNNNX` | Montant (13 chiffres + signe)            |
| 105-120  | 16       | ` `              | Zone réservée                            |

#### Enregistrement 04 - Opération

| Position | Longueur | Contenu          | Description                   |
|----------|----------|------------------|-------------------------------|
| 1-2      | 2        | `04`             | Code enregistrement           |
| 3-7      | 5        | `NNNNN`          | Code banque                   |
| 8-11     | 4        | `XXXX`           | Code opération interne        |
| 12-16    | 5        | `NNNNN`          | Code guichet                  |
| 17-19    | 3        | `EUR`            | Code devise (ISO)             |
| 20       | 1        | `N`              | Nombre de décimales           |
| 21       | 1        | ` `              | Zone réservée                 |
| 22-32    | 11       | `NNNNNNNNNNN`    | Numéro de compte              |
| 33-34    | 2        | `XX`             | Code opération interbancaire  |
| 35-40    | 6        | `JJMMAA`         | Date opération                |
| 41-43    | 3        | `NNN`            | Code motif de rejet           |
| 44-49    | 6        | `JJMMAA`         | Date valeur                   |
| 50-80    | 31       | `TEXTE`          | Libellé opération             |
| 81-82    | 2        | ` `              | Zone réservée                 |
| 83-89    | 7        | `NNNNNNN`        | Numéro d'écriture             |
| 90       | 1        | `X`              | Indice exonération commission |
| 91       | 1        | `X`              | Indice d'indisponibilité      |
| 92-104   | 13       | `NNNNNNNNNNNNNX` | Montant (avec le signe)       |
| 105-120  | 16       | `TEXTE`          | Référence                     |

> **Note** : La zone "Code opération interne" est propre à chaque établissement bancaire. Elle permet à la banque d'identifier le type d'opération dans son système interne.

> **Attention** : Le montant du mouvement fait **13 caractères**, contrairement au montant des soldes qui font **14 caractères**. Cette différence s'explique par la présence des zones indices en positions 90 et 91.

#### Enregistrement 05 - Complément à l'enregistrement 04

Cet enregistrement **facultatif** permet d'ajouter des informations complémentaires à une opération (04). Il peut y avoir plusieurs enregistrements 05 après un même 04.

| Position | Longueur | Contenu          | Description                            |
|----------|----------|------------------|----------------------------------------|
| 1-2      | 2        | `05`             | Code enregistrement                    |
| 3-40     | 38       | *(identique 04)* | Même structure que l'enregistrement 04 |
| 41-45    | 5        | `     `          | Zone réservée                          |
| 46-48    | 3        | `XXX`            | Qualifiant                             |
| 49-118   | 70       | `TEXTE`          | Informations complémentaires           |
| 119-120  | 2        | `  `             | Zone réservée                          |

**Qualifiants courants :**
- **`LIB` : Libellé complémentaire**
- **`MMO` : Montant et monnaie d'origine**
- `LCC` : Libellé complémentaire (variante)
- `NBE` : Nom du bénéficiaire/émetteur
- `REF` : Référence de l'opération
- `BDB` : BIC de la banque du bénéficiaire

#### Enregistrement 07 - Nouveau solde

Structure identique à l'enregistrement 01, avec le code `07`.

### 4.5 Encodage des montants

**Point critique** : Le dernier caractère du montant encode à la fois le chiffre des unités ET le signe *(crédit/débit)*.

| Dernier caractère | Chiffre | Signe |
|-------------------|---------|-------|
| `{` ou `A`-`I` | 0-9 | **Crédit** (+) |
| `}` ou `J`-`R` | 0-9 | **Débit** (−) |

**Table de correspondance complète :**

| Crédit | `{` | `A` | `B` | `C` | `D` | `E` | `F` | `G` | `H` | `I` |
|--------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| Valeur | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |

| Débit | `}` | `J` | `K` | `L` | `M` | `N` | `O` | `P` | `Q` | `R` |
|-------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| Valeur | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |


**Exemple 1** : `0000001925107{` → **19 251,07 € (crédit)**

1. Les 13 premiers caractères : `0000001925107`
2. Le caractère `{` indique :
    - Chiffre : **0**
    - Signe : **crédit** (+)
3. Montant complet : `0000001925107` & `0` = `00000019251070`
4. Avec 2 décimales : 19 251 070 / 100 = **19 251,07 € (crédit)**


**Exemple 2** : `0000000139719C` → **13 971,93 € (crédit)**

1. Les 13 premiers caractères : `0000000139719`
2. Le caractère `C` indique :
    - Chiffre : **3**
    - Signe : **crédit** (+)
3. Montant complet : `0000000139719` & `3` = `00000001397193`
4. Avec 2 décimales : 1 397 193 / 100 = **13 971,93 € (crédit)**


**Exemple 3** : `0000000002421}` → **242,10 € (débit)**

1. Les 13 premiers caractères : `0000000002421`
2. Le caractère `}` indique :
    - Chiffre : **0**
    - Signe : **débit** (−)
3. Montant complet : `0000000002421` & `0` = `00000000024210`
4. Avec 2 décimales : 24 210 / 100 = **242,10 € (débit)**

> **Attention** : Une erreur sur le décodage du signe inverse le sens de l'opération !

---

## 5. Spécifications fonctionnelles

### 5.1 Fonctionnalités obligatoires (MVP)

Votre programme doit implémenter les fonctionnalités suivantes :

#### F1 - Chargement d'un fichier CFONB120

```bash
./cfonb_analyzer load <fichier.txt>
```

- Lit le fichier ligne par ligne
- Parse chaque enregistrement selon son type
- Stocke les données dans les structures appropriées
- Affiche un résumé : nombre d'enregistrements par type

#### F2 - Validation des données

```bash
./cfonb_analyzer validate <fichier.txt>
```

- Vérifie la cohérence des enregistrements (01 → 04/05 → 07)
- Contrôle que chaque bloc commence par 01 et finit par 07
- Vérifie la correspondance des numéros de compte dans un bloc
- Recalcule le solde final et le compare au solde déclaré en 07
- Affiche les anomalies détectées

#### F3 - Extraction de statistiques

```bash
./cfonb_analyzer stats <fichier.txt>
```

Affiche pour chaque compte :
- Numéro de compte et titulaire
- Solde initial et solde final
- Nombre d'opérations (débits / crédits)
- Total des débits et total des crédits
- Variation nette sur la période

#### F4 - Recherche d'opérations

```bash
./cfonb_analyzer search <fichier.txt> --compte <numero>
./cfonb_analyzer search <fichier.txt> --montant-min <valeur>
./cfonb_analyzer search <fichier.txt> --date <JJMMAA>
```

Filtre et affiche les opérations correspondant aux critères.

### 5.2 Fonctionnalités bonus

Les fonctionnalités suivantes rapportent des points supplémentaires :

| Bonus | Description | Points |
|-------|-------------|--------|
| B1    | Export CSV des opérations | +1.5   |
| B2    | Traitement de plusieurs fichiers (dossier) | +1.5   |
| B3    | Détection des doublons d'opération | +1     |
| B4    | Tri des opérations par date/montant | +1     |

---

## 6. Architecture imposée

### 6.1 Structure du projet

```
CFONB_Analyzer/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── cfonb_types.h      # Définitions des types et structures
│   ├── cfonb_parser.h     # Fonctions de parsing
│   ├── cfonb_validator.h  # Fonctions de validation
│   ├── cfonb_stats.h      # Fonctions statistiques
│   └── cfonb_utils.h      # Utilitaires (dates, montants)
├── src/
│   ├── main.c             # Point d'entrée, gestion des arguments
│   ├── cfonb_parser.c     # Implémentation du parser
│   ├── cfonb_validator.c  # Implémentation de la validation
│   ├── cfonb_stats.c      # Implémentation des statistiques
│   └── cfonb_utils.c      # Implémentation des utilitaires
├── data/
│   └── (fichiers CFONB fournis)
└── tests/
    └── (vos fichiers de test)
```

### 6.2 Fichier CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)
project(CFONB_Analyzer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

include_directories(include)

add_executable(cfonb_analyzer
    src/main.c
    src/cfonb_parser.c
    src/cfonb_validator.c
    src/cfonb_stats.c
    src/cfonb_utils.c
)
```

---

## 7. Types et structures de données

### 7.1 Énumérations

```c
// Types d'enregistrements CFONB
typedef enum {
    RECORD_OLD_BALANCE = 1,    // 01 - Ancien solde
    RECORD_OPERATION = 4,      // 04 - Opération
    RECORD_COMPLEMENT = 5,     // 05 - Complément
    RECORD_NEW_BALANCE = 7     // 07 - Nouveau solde
} RecordType;

// Sens de l'opération
typedef enum {
    SENS_CREDIT,    // Entrée d'argent
    SENS_DEBIT      // Sortie d'argent
} SensOperation;

// Codes retour pour la validation
typedef enum {
    VALID_OK = 0,
    VALID_ERR_SEQUENCE,       // Séquence 01-04-07 non respectée
    VALID_ERR_COMPTE,         // Numéro de compte incohérent
    VALID_ERR_SOLDE,          // Solde recalculé != solde déclaré
    VALID_ERR_FORMAT          // Erreur de format
} ValidationResult;
```

### 7.2 Structures principales

```c
// Date au format CFONB (JJMMAA)
typedef struct {
    unsigned int jour  : 5;  // 1-31  → max 31, besoin de 5 bits
    unsigned int mois  : 4;  // 1-12  → max 12, besoin de 4 bits
    unsigned int annee : 7;  // 0-99  → max 99, besoin de 7 bits (25 = 2025)
} DateCFONB;  // Total : 16 bits (2 octets)

// Montant avec signe
typedef struct {
    unsigned long centimes;        // Montant en centimes (évite les flottants)
    SensOperation sens;            // Crédit ou débit
} Montant;

// Informations de compte (enregistrements 01 et 07)
typedef struct {
    char codeBanque[6];      // 5 + \0
    char codeGuichet[6];     // 5 + \0
    char devise[4];          // 3 + \0 (EUR)
    int nbDecimales;
    char numeroCompte[12];   // 11 + \0
    DateCFONB date;
    char titulaire[51];      // 50 + \0
    Montant solde;
} InfoCompte;

// Une opération (enregistrement 04)
typedef struct {
    char numeroCompte[12];
    char codeOperation[3];   // 2 + \0
    DateCFONB dateOperation;
    DateCFONB dateValeur;
    char libelle[32];        // 31 + \0
    Montant montant;
    char reference[17];      // 16 + \0
    // Compléments éventuels (04 peut avoir plusieurs 05)
    char complements[5][71]; // Max 5 compléments de 70 chars
    int nbComplements;
} Operation;

// Bloc complet d'un compte (01 + opérations + 07)
typedef struct {
    InfoCompte ancienSolde;    // Enregistrement 01
    Operation* operations;     // Tableau dynamique d'opérations
    int nbOperations;
    int capaciteOperations;    // Pour la gestion dynamique
    InfoCompte nouveauSolde;   // Enregistrement 07
} BlocCompte;

// Fichier CFONB complet
typedef struct {
    char nomFichier[256];
    BlocCompte* blocs;         // Tableau dynamique de blocs
    int nbBlocs;
    int capaciteBlocs;
} FichierCFONB;
```

### 7.3 Structures de résultats

```c
// Statistiques d'un compte
typedef struct {
    char numeroCompte[12];
    char titulaire[51];
    Montant soldeInitial;
    Montant soldeFinal;
    int nbDebits;
    int nbCredits;
    long totalDebits;       // En centimes
    long totalCredits;      // En centimes
    long variation;         // Credits - Debits
} StatsCompte;

// Rapport de validation
typedef struct {
    ValidationResult resultat;
    int ligneErreur;
    char messageErreur[256];
} RapportValidation;
```

---

## 8. Travail demandé

### Phase 0 - Télécharger les données

**Téléchargez l'archive** : https://umbox.univ-lemans.fr/index.php/s/NTdZLsPjPnkFP5B

**Contenu de l'archive** :
1. **CFONB120_specification.pdf** : Documentation technique du format
2. **data/** : Dossier contenant 149 fichiers CFONB anonymisés (Données brut)

> **Extraction** : Décompressez l'archive à la racine de votre projet.

> **Attension** : Vérifiez bien la présence de `data/*` dans votre fichier `.gitignore`. On ne pousse jamais de données sensibles ou de jeux de données issus de vraies données sur GitHub, même anonymisées.

### Phase 1 - Analyse et conception

Avant de coder, vous devez :

1. **Lire intégralement** la spécification CFONB120 fournie
2. **Analyser** les fichiers exemples dans le dossier `data/`
3. **Dessiner** sur papier le flux de données de votre programme
4. **Identifier** les cas limites et erreurs possibles

> **Livrable** : Un fichier `CONCEPTION.md` décrivant votre analyse (facultatif mais valorisé)

### Phase 2 - Implémentation des utilitaires

Commencez par les fonctions de bas niveau dans `cfonb_utils.c` :

```c
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
```

**Conseil** : Testez chaque fonction individuellement avant de continuer.

### Phase 3 - Implémentation du parser

Dans `cfonb_parser.c`, implémentez :

```c
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
```

### Phase 4 - Implémentation de la validation

Dans `cfonb_validator.c` :

```c
// Valide la structure d'un bloc (séquence 01-04-07)
RapportValidation validerStructureBloc(BlocCompte* bloc);

// Vérifie la cohérence du numéro de compte
RapportValidation validerCoherenceCompte(BlocCompte* bloc);

// Recalcule et vérifie le solde
RapportValidation validerSolde(BlocCompte* bloc);

// Validation complète d'un fichier
RapportValidation* validerFichier(FichierCFONB* fichier, int* nbRapports);
```

### Phase 5 - Implémentation des statistiques

Dans `cfonb_stats.c` :

```c
// Calcule les stats d'un bloc
StatsCompte calculerStatsBloc(BlocCompte* bloc);

// Affiche les stats de tous les comptes
void afficherStatsFichier(FichierCFONB* fichier);

// Recherche des opérations selon critères
Operation** rechercherOperations(FichierCFONB* fichier,
                                  const char* numeroCompte,
                                  long montantMin,
                                  DateCFONB* date,
                                  int* nbResultats);
```

### Phase 6 - Programme principal

Dans `main.c`, implémentez le point d'entrée qui :

1. Parse les arguments de la ligne de commande
2. Appelle les fonctions appropriées
3. Affiche les résultats
4. Gère proprement les erreurs

---

## 9. Contraintes techniques

### 9.1 Règles de codage obligatoires

| Règle | Description                                                  |
|-------|--------------------------------------------------------------|
| **C11** | Standard C11 obligatoire                                     |
| **Warnings** | Compilation avec `-Wall -Wextra -Werror` sans erreur         |
| **Mémoire** | Toute allocation doit être libérée                           |
| **Fonctions** | Maximum 50 à 80 lignes par fonction                          |
| **Nommage** | camelCase pour les fonctions, MAJUSCULES pour les constantes |
| **Commentaires** | Chaque fonction doit avoir un commentaire d'en-tête          |

### 9.2 Gestion des erreurs

Votre programme doit gérer gracieusement :

- Fichier inexistant ou illisible
- Ligne de longueur incorrecte (≠ 120 caractères)
- Type d'enregistrement inconnu
- Format de montant invalide
- Date invalide
- Arguments manquants en ligne de commande

**Ne jamais** : crasher, boucler infiniment, ou afficher des messages incompréhensibles.

### 9.3 Qualité du code

Votre code sera évalué sur :

- **Lisibilité** : Peut-on comprendre le code sans effort ?
- **Modularité** : Les responsabilités sont-elles bien séparées ?
- **Robustesse** : Le code gère-t-il les cas limites ?
- **Efficacité** : Les algorithmes sont-ils raisonnables ?
- **Documentation** : Le code est-il bien commenté ?

---

## 10. Conseils d'implémentation

### 10.1 Parsing des champs fixes

Les positions dans la norme CFONB sont en **base 1**. En C, les tableaux commencent à 0.

```c
// Position 22-32 en CFONB (numéro de compte) : indices 21 à 31 en C
void extraireChamp(const char* ligne, int debut, int fin, char* dest) {
    int longueur = fin - debut + 1;
    strncpy(dest, ligne + debut - 1, longueur);
    dest[longueur] = '\0';
    // Retirer les espaces de fin (trim)
    while (longueur > 0 && dest[longueur - 1] == ' ') {
        dest[--longueur] = '\0';
    }
}
```

### 10.2 Décodage du montant signé

```c
Montant decoderMontant(const char* montantStr, int nbDecimales) {
    Montant m;
    int longueur = strlen(montantStr);  // 14 pour types 01/07, 13 pour type 04
    char dernierChar = montantStr[longueur - 1];

    // Extraire les (longueur-1) premiers caractères comme nombre
    char partie[15];
    strncpy(partie, montantStr, longueur - 1);
    partie[longueur - 1] = '\0';
    long valeur = atol(partie) * 10;  // Multiplier par 10 pour le dernier chiffre

    // Décoder le dernier caractère
    if (dernierChar == '{') {
        m.sens = SENS_CREDIT;
        valeur += 0;
    } else if (dernierChar >= 'A' && dernierChar <= 'I') {
        m.sens = SENS_CREDIT;
        valeur += (dernierChar - 'A' + 1);
    } else if (dernierChar == '}') {
        m.sens = SENS_DEBIT;
        valeur += 0;
    } else if (dernierChar >= 'J' && dernierChar <= 'R') {
        m.sens = SENS_DEBIT;
        valeur += (dernierChar - 'J' + 1);
    }
    
    // Normaliser en centimes (Déjà en centimes si nbDecimales = 2)
    if (nbDecimales == 0) {
        valeur *= 100;  // Pas de décimales → multiplier par 100
    } else if (nbDecimales == 1) {
        valeur *= 10;   // 1 décimale → multiplier par 10
    }

    m.centimes = valeur;
    return m;
}
```

### 10.3 Allocation dynamique

```c
// Initialisation d'un fichier CFONB
FichierCFONB* creerFichier(const char* nom) {
    FichierCFONB* f = malloc(sizeof(FichierCFONB));
    if (!f) return NULL;

    strncpy(f->nomFichier, nom, 255);
    f->nomFichier[255] = '\0';

    f->capaciteBlocs = 10;  // Capacité initiale
    f->blocs = malloc(f->capaciteBlocs * sizeof(BlocCompte));
    f->nbBlocs = 0;

    return f;
}

// Ajouter un bloc (avec réallocation si nécessaire)
int ajouterBloc(FichierCFONB* fichier, BlocCompte bloc) {
    if (fichier->nbBlocs >= fichier->capaciteBlocs) {
        fichier->capaciteBlocs *= 2;
        BlocCompte* nouveau = realloc(fichier->blocs,
                                       fichier->capaciteBlocs * sizeof(BlocCompte));
        if (!nouveau) return -1;
        fichier->blocs = nouveau;
    }
    fichier->blocs[fichier->nbBlocs++] = bloc;
    return 0;
}
```

### 10.4 Validation du solde

```c
RapportValidation validerSolde(BlocCompte* bloc) {
    RapportValidation rapport = { VALID_OK, 0, "" };

    // Partir du solde initial
    long soldeCalcule = bloc->ancienSolde.solde.centimes;
    if (bloc->ancienSolde.solde.sens == SENS_DEBIT) {
        soldeCalcule = -soldeCalcule;
    }

    // Appliquer chaque opération
    for (int i = 0; i < bloc->nbOperations; i++) {
        long montant = bloc->operations[i].montant.centimes;
        if (bloc->operations[i].montant.sens == SENS_CREDIT) {
            soldeCalcule += montant;
        } else {
            soldeCalcule -= montant;
        }
    }

    // Comparer avec le solde déclaré
    long soldeDeclare = bloc->nouveauSolde.solde.centimes;
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
```

---

## 11. Consignes de rendu GitHub

### 11.1 Création du dépôt

1. Créez un dépôt **privé** nommé `CFONB_Analyzer_NOM_Prenom`
2. Ajoutez votre enseignant comme collaborateur : `jessy70210`
3. Ajoutez l’URL de votre dépôt dans le fichier partagé : https://docs.google.com/spreadsheets/d/1gICmBXFKXNyQSnRVe6RoVB5GRI6rC8-ab24fstwkouo/edit?usp=sharing

### 11.2 Structure des commits

Utilisez des commits **atomiques** avec des messages explicites :

```bash
# BON
git commit -m "feat: implémentation du décodage des montants signés"
git commit -m "fix: correction de l'extraction de la date"
git commit -m "test: ajout des tests pour le parser"

# MAUVAIS
git commit -m "modifications"
git commit -m "ça marche"
git commit -m "fix bug"
```

### 11.3 Branches recommandées (Optionnel pour le TP, mais qualité pro)

```
main
├── feature/parser
├── feature/validator
├── feature/stats
└── feature/bonus-csv
```

### 11.4 Fichier README.md obligatoire

Votre README doit contenir :

1. **Compilation** : Instructions pour compiler le projet
2. **Utilisation** : Exemples de commandes
3. **Fonctionnalités** : Liste des fonctionnalités implémentées
4. **Difficultés rencontrées** : Section réflexive (optionnelle mais valorisée)

### 11.5 Date limite

**Rendu avant : 04 janvier 2026**

---

## 12. Barème de notation

### 12.1 Répartition des points (sur 20)

| Critère | Points | Détail |
|---------|--------|--------|
| **Compilation** | 2 | Compile sans erreur ni warning |
| **Parser (F1)** | 4 | Chargement correct des fichiers |
| **Validation (F2)** | 4 | Détection des anomalies |
| **Statistiques (F3)** | 3 | Calculs corrects |
| **Recherche (F4)** | 2 | Filtres fonctionnels |
| **Qualité du code** | 3 | Lisibilité, modularité, robustesse |
| **Git & Documentation** | 2 | Commits, README, structure |
| **Bonus** | +5 max | Voir section 5.2 |

### 12.2 Malus

| Infraction | Pénalité |
|---------|----------|
| Fuite mémoire | -2 |
| Crash sur fichier valide | -3 |
| Plagiat (même partiel) | -20 |
| Rendu en retard | -2 par jour |

### 12.3 Critères de qualité de code

| Niveau | Description | Points |
|--------|-------------|--------|
| **Insuffisant** | Code désordonné, pas de gestion d'erreur | 0 |
| **Passable** | Code fonctionnel mais difficile à lire | 1 |
| **Bien** | Code propre, bien structuré | 2 |
| **Excellent** | Code professionnel, documentation complète | 3 |

---

## Annexe A - Tables de codes

### A.1 Codes opération interbancaires (position 33-34)

| Code | Signification |
|------|---------------|
| `02` | Chèque |
| `06` | Carte bancaire |
| `12` | Virement reçu |
| `13` | Virement émis |
| `17` | Prélèvement |
| `27` | Frais bancaires |

### A.2 Exemple de codes motif de rejet (position 41-43)

| Code | Signification |
|------|---------------|
| `000` | Pas de rejet |
| `001` | Provision insuffisante |
| `002` | Compte clôturé |
| `003` | Opposition |

---

## Annexe B - Exemples de sorties attendues

### B.1 Commande `load`

```
$ ./cfonb_analyzer load data/SOBANK_2025-11-30.txt

=== CHARGEMENT CFONB ===
Fichier: data/SOBANK_2025-11-30.txt
Enregistrements lus: 17
  - Type 01 (ancien solde): 5
  - Type 04 (opérations): 2
  - Type 05 (compléments): 6
  - Type 07 (nouveau solde): 5

Blocs de comptes détectés: 5
Chargement réussi.
```

### B.2 Commande `validate`

```
$ ./cfonb_analyzer validate data/SOBANK_2025-11-30.txt

=== VALIDATION CFONB ===
Fichier: data/SOBANK_2025-11-30.txt

Bloc 1 - Compte 02863280452
  Structure valide  Ok
  Cohérence compte  Ok
  Solde vérifié     Ok

Bloc 2 - Compte 02378420485
  Structure valide  Ok
  Cohérence compte  Ok
  Solde vérifié     OK

[...]

Résultat: 5/5 blocs valides
```

### B.3 Commande `stats`

```
$ ./cfonb_analyzer stats data/SOBANK_2025-11-30.txt

=== STATISTIQUES CFONB ===

Compte: 02863280452 - SIVASOORIYALINGAM SIVALOGINI
  Solde initial: 13 971,93 € (CR)
  Solde final:   13 971,93 € (CR)
  Opérations:    0 débits / 0 crédits
  Total débits:  0,00 €
  Total crédits: 0,00 €
  Variation:     0,00 €

Compte: 02150880432 - DE FI CO
  Solde initial: 192 510,70 € (CR)
  Solde final:   192 450,70 € (CR)
  Opérations:    1 débit / 0 crédit
  Total débits:  60,00 €
  Total crédits: 0,00 €
  Variation:     -60,00 €

[...]
```

### B.4 Commande `search`

```
$ ./cfonb_analyzer search data/SOBANK_2025-11-30.txt --compte 02150880432

=== RECHERCHE D'OPÉRATIONS ===
Critère: Compte = 02150880432

Résultats: 1 opération(s) trouvée(s)

Date       | Montant      | Libellé
-----------|--------------|--------------------------------
29/11/2025 | -60,00 €     | CARTE 28/11 LE SQUARE KG PARIS
           |              | > MCC 5812 RESTAUR
```

---

## Ressources fournies

Les fichiers suivants sont fournis avec ce TP :

1. `./CFONB120_specification.pdf` - Documentation technique complète du format
2. `./data/` - Dossier contenant 149 fichiers CFONB anonymisés

Téléchargeable ici : https://umbox.univ-lemans.fr/index.php/s/NTdZLsPjPnkFP5B

---

**Bon courage !**

*Ce TP simule une situation réelle. La rigueur et la qualité de votre code reflètent votre professionnalisme.*

