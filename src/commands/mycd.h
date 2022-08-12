// les includes qui vont bien..
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

// les constantes.
#define BUFFERSIZE 1024

// variable globale du chemin courant.
char cwd[BUFFERSIZE];

// permet de se déplacer dans le répertoire passé en paramètre de cette commande.
int mycd(int argc, char *argv[]); 