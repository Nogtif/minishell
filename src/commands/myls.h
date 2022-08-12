// les includes qui vont bien...
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// la librarie locale des fonctions utilitaires.
#include "../utils.h"

// converti le mode d'un fichier en chaîne de caractère.
void modeToString(int mode, char *str);

// affiche le fichier avec l'ensemble de ses données.
void printFile(char const *namefile, struct stat data);

// affiche un message d'aide sur la sortie standard.
void mylsHelp();

// liste le contenu d'un répertoire de manière détaillé par (-l) en fonction des options.
void readDirectory(const char *filename);

// fonction principale qui exécute la commande myls.
int myls(int argc, char *argv[]);
