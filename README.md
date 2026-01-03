# Compte-rendu de TP
**1. Objectif général du projet**

Ce projet a pour objectif de lire, interpréter, structurer et valider un fichier au format CFONB, selon une logique modulaire et progressive.
Le traitement repose sur une séparation claire entre :
Le parsing (lecture et structuration des données),
la validation (vérification de la cohérence logique et comptable),
L'exploitation des données (recherche et affichage).
Cette séparation garantit une meilleure maintenabilité, une lisibilité accrue du code et une évolution facilitée des règles métiers.

**2. Logique générale du parsing**

Le parsing du fichier CFONB repose sur une lecture séquentielle stricte, respectant la structure officielle du format CFONB.

**2.1 Principe fondamental**

Le fichier est lu ligne par ligne, et chaque ligne est interprétée selon son type :

* 01 : ancien solde

* 04 : opération

* 05 : complément d’opération

* 07 : nouveau solde

Le parsing ne réalise aucune validation métier à ce stade. Il se limite à :

* Identifier les types d’enregistrements,

* Extraire les champs,

* Structurer les données dans des entités cohérentes.

Ce choix permet de séparer clairement la responsabilité de lecture de celle de validation, évitant un couplage excessif entre logique métier et logique d’entrée/sortie.

**3. Construction des blocs (BlocCompte)**

Le cœur de l’architecture repose sur la notion de bloc de compte.
Un bloc est constitué de :

* Un ancien solde (01),

* Aucune ou plusieurs opérations (04),

* Eventuellement des compléments d’opérations (05),

* Un nouveau solde (07).

Logique retenue :
Le parsing démarre toujours par la lecture d’un enregistrement (01).
Les lignes suivantes sont traitées comme des opérations tant qu’un 07 n’est pas rencontré.
Les compléments (05) sont rattachés à la dernière opération lue.
La lecture du 07 marque la fin du bloc.
Ce fonctionnement garantit que le bloc est structurellement cohérent, indépendamment de sa validité métier.

**4. Séparation parsing / validation**

Un choix fondamental du projet a été de ne jamais valider pendant le parsing.
Raisons :
Le parsing doit rester tolérant : il construit les structures même si les données sont incorrectes.
Toute la logique de vérification est centralisée dans les fonctions de validation.
Cela permet d’avoir une vision globale et cohérente des erreurs plutôt qu’un arrêt prématuré.
Ainsi, un bloc est toujours créé, même s’il contient des incohérences.

**5. Validation des données**

La validation est subdivisée en trois axes distincts :

**5.1 Validation de la structure du bloc**

Cette validation vérifie :

* La présence d’un ancien solde valide,

* La présence d’un nouveau solde valide,

* La cohérence des champs obligatoires (longueur du numéro de compte, code banque, titulaire),

* La validité des dates d’opérations,

* L'existence de montants non nuls pour les opérations.

Cette étape garantit que le bloc respecte la structure attendue d’un CFONB.

**5.2 Validation de la cohérence des numéros de compte**

Cette validation s’appuie sur les règles suivantes :

* Le numéro de compte de l’ancien solde doit être valide,

* Le numéro de compte du nouveau solde doit correspondre exactement à celui de l’ancien solde,

* Toutes les opérations du bloc doivent référencer le même numéro de compte.

Ce choix permet de détecter toute incohérence interne au bloc.

**5.3 Validation du solde**

La validation du solde repose sur un recalcul complet :
On part du solde initial.
On applique successivement toutes les opérations (débit/crédit).
On compare le solde obtenu au solde final déclaré.
Ce mécanisme garantit l’intégrité financière du bloc.

**6. Architecture des rapports de validation**

Chaque validation produit un RapportValidation contenant :

* Un statut (OK ou erreur),

* Un message explicite,

* Une information de localisation (ligne ou opération concernée).

Les rapports sont regroupés par bloc, ce qui permet :

Un affichage structuré,
Une vision claire des erreurs,
Une indépendance entre les différentes vérifications.

**7. Philosophie générale du projet**

Les choix d’architecture reposent sur les principes suivants :
Séparation claire des responsabilités (parsing / validation / affichage)
Lisibilité et maintenabilité du code
Tolérance à l’erreur lors de la lecture
Validation explicite et localisée
Aucune dépendance implicite entre les étapes
Cette approche permet d’obtenir un système robuste, extensible et facilement débogable.

**8. Conclusion**

L’architecture mise en place permet de traiter un fichier CFONB de manière fiable et structurée.
Chaque étape a une responsabilité bien définie, ce qui facilite :
La compréhension du code,
La maintenance,
L'évolution future (ajout de règles, nouveaux contrôles, nouveaux formats).
Le projet respecte ainsi une logique professionnelle de traitement de données financières structurées.

**9. Difficulté rencontrée**

La gestion fine de la lecture séquentielle des lignes du fichier CFONB est à évaluer avec précaution.
Dans mon cas, je rencontre un défaut de lecture des compléments de la premiere opération de chaque bloc.
Afin de palier à ce défaut, lors du parsing, j'ai structuré le flux de lecture dans l'optique d'évaluer le type de ligne 
après chaque lecture d'une ligne. Jusque là, je n'ai toujours pas pu remedier à cette anomalie. Cet élément 
fut juste constaté sans rétroaction afin de ne pas créer de disfonctionnements dans le programme global.