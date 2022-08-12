// les includes qui vont bien..
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// la librarie locale des fonctions utilitaires.
#include "../utils.h"

// constantes..
#define BUFFER_SIZE 65536

/** Structure qui modélise un buffer de lecteur de fichier
 * avec son statut de lecture et son contenu dans un buffer.
 */
typedef struct _bufferReader {
  int fd;
  char *buffer;
} bufferReader;

/** Structure qui regroupe l'ensemble des informations
 * nécessaire pour un processus en cours.
 */
typedef struct {
  int pid, vsz, rss;
  char *user, cmd[256], state, tty[8], time[5], startTime[5];
  float cpu, mem;
} dataProc;

// fonction qui lit un fichier et crée un bufferReader pour y stocker son contenu.
bufferReader *createBufferReader(char *filename);

// détruit le bufferReader du fichier passé en paramètre.
void destroyBufferReader(bufferReader *data);

// affiche les informations d'un procussus.
void printProcessus(dataProc proc);

// affiche un message d'aide sur la sortie standard.
void mypsHelp();

// parcourt le dossier /proc afin de lire chaque dossier de processus.
void readProcessus();

// fonction principale qui exécute la commande myps.
int myps(int argc, char *argv[]);