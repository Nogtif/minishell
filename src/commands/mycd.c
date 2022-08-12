/** Programme qui implémente la commande mycd. 
 */

// la librairie pour la commande mycd.
#include "mycd.h"

/** Fonction principale qui s'occupe d'exécuter la commande mycd.
 * @param argc : le nombre de paramètres.
 * @param argv : tableau de chaine de caractères contenant les parmaètres.
 * @return : la valeur de retour (0) si tout c'est bien passé.
 */
int mycd(int argc, char *argv[]) {
  // quelques variables..
  char cwd[BUFFERSIZE];
  int res;

  // vérifie que nous avons au maximum 1 argument qui est le répertoire.
  if(argc > 2) perror("Trop d'arguments..."), exit(1);

  // si il n'y a pas de répertoire donné, on déplace vers la home directory.
  if(argc == 1) {
    sprintf(cwd, "/home/%s", getenv("LOGNAME"));
  }
  // si ce n'est pas un nouveau chemin, on concat avec le chemin courant.
  else if(argv[1][0] != '/') {
    getcwd(cwd, sizeof(cwd));
    strcat(cwd,"/");
    strcat(cwd, argv[1]);
  }
  // sinon on écrase l'ancien chemin par le nouveau.  
  else {
    sprintf(cwd, "%s", argv[1]);
  }

  // en cas de répertoire introuvable..
  if((res = chdir(cwd)) == -1) perror("mycd: erreur"), exit(1);

  // renvoie la valeur de retour de chdir.
  return res;
}