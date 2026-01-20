# Système de traitement CFONB120

## Présentation
Ce projet implémente en langage **C** un système de traitement de fichiers bancaires au format **CFONB120**.  
Il permet de charger, valider et exploiter les données d’un relevé bancaire via une interface en ligne de commande.

---

## Méthodes d’implémentation

- **Architecture modulaire** : séparation claire des responsabilités  
  (parser, validation, statistiques, utilitaires)
- **Parsing structuré** : extraction des champs par positions fixes selon la norme CFONB120
- **Contrôles de cohérence** : ordre des enregistrements, soldes et opérations
- **Exploitation fonctionnelle** : calcul de statistiques et analyse des opérations
- **Robustesse** : compilation stricte, gestion des erreurs et de la mémoire


