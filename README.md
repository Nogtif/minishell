# Mini-shell

## 1. Le projet

L'objectif du projet était de réaliser un programme C qui modélise un shell capable d’exécuter des programmes.
Le projet se compose d'un dossier `src` contenant l'ensemble du code source avec des fichiers *.c* et *.h*. Au sein de ce dossier, se trouve également un sous-dossier `commands` regroupant le code pour les commandes myps, myls et mycd.

Pour une meilleure organisation dans le code, nous avons divisé celui-ci en plusieurs fichiers :

- **main.c** : le fichier qui lance le programme principal.
- **myshell.c** : le fichier qui regroupe les principales méthodes au bon déroulement du programme shell (exécution des commandes, vérifications, ...).
- **utils.c** : regroupant les fonctions utilitaires pouvant être utilisé dans plusieurs fichiers.
- **variable.c** : le fichier qui gère les variables locales du shell avec ses méthodes set/echo/unset.

*Chacun de ces fichiers à une librairie locale qui lui est associée contenant les includes qui vont bien et les déclarations des fonctions.*

#### Compiler et lancer le projet

Pour compiler le programme, rendez dans vôtre terminal et exécuter la compilation grâce au Makefile.
```
$ make
```

Une fois le programme compiler et l'exécutable généré à la racine du projet, il suffit de lancer la commande suivante.
```
$ ./mysh
```

*Note: pour nettoyer le projet, lancez la commande: ```make clean```*

## 2. Les fonctionnalités

Ci-dessous vous trouverez un tableau qui récapitule l'ensemble des fonctionnalités implémentées (ou non) dans le projet.

| Fonctionnalités | Implémentée? | Note | Par qui? |
| --------------- | ------------ | ---- | -------- |
| **Lancement de commandes** |            |      |
| Séquencement | ✔️ | le séparateur `;` doit être espacé entre deux séquences (comme dans l'exemple du sujet). |
| Wildcards | ✔️ | - |
| **Commandes** |  |  |
| Changement de répertoire | ✔️ | - |
| Sortie du Shell (et propagation du Ctrl-C) | ➖ | Le Ctrl+C ne kill pas un processus en cours. |
| Code de retour d’un processus | ✔️ | - |
| Lister le contenu d’un répertoire | ✔️ | - |
| Afficher l’état des processus en cours | ✔️ | - |
| **Les redirections** |  |  | |
| Les tubes | ❌ | trop de bugs, pas implémenté mais dispo dans une branch du Git |
| Redirections vers ou depuis un fichier | ✔️ | - |
| **Premier et arrière plans** |  | Bcp de bugs, erreur valgrind donc pas implémentée. |
| Commande myjobs | ✔️ | implémenté mais ne sert à rien car il n'y a pas le reste. |
| Passer une commande de foreground à background et inversement | ❌ | pas implémentée |
| **Les variables** |  |  |
| Les variables d’environnment | ❌ | pas de variables d'environnement. (pas réussi) |
| Les variables locales | ✔️ | - |
| Utilisation des variables | ➖ | la partie utilisation avec les variables locales est fonctionnel mais pas avec ceux d'environnement. |

