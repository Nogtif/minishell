// on importe quelques librairies...
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

#include "utils.h"

#define NAME_FILE "/bin/bash"
#define NUM_SHM 1664
#define NUM_SEM1 51
#define NUM_SEM2 27
#define NUM_MUTEX 31

/** Structure du définit une variable.
 * > name : son nom.
 * > value: sa valeur.
 */
typedef struct {
  char *name;
  char *value;
} Variable;

/** Structure qui modélise une liste chaînée de variable.
 * avec sa variable et son suivant.
 */
typedef struct _list {
  Variable *var;
  struct _list *next;
} List;

/** Structure qui modélise un environnement avec
 * le nombre d'utilisateurs, de fragments, de lecteurs et d'écrivains.
*/
typedef struct {
  int users;
  int fragments;
  int readers;
  int writers;
} Environment;

// initialise l'environnement de mysh.
int initEnvironment();

/** VARIABLES GLOBALES */

// renvoie la valeur du nom de la variable demandée.
char *getGlobalVariable(char *name);

// affiche l'ensemble des variables locales.
void printGlobalVariables();

// déclare une nouvelle variable (écrase l'ancienne valeur si elle existe déjà).
int setGlobalVariable(char *name, char *val);

// supprime la variable de la liste.
int unsetGlobalVariable(char *name);

/** VARIABLES LOCALES */

// renvoie la valeur du nom de la variable demandée.
char *getLocalVariable(char *name);

// affiche l'ensemble des variables locales.
void printLocalVariables();

// déclare une nouvelle variable (écrase l'ancienne valeur si elle existe déjà).
int setLocalVariable(char *newVar);

int echoVariable(char *var);

// supprime la variable de la liste.
int unsetLocalVariable(char *name);

// vide la liste des variables locales.
void cleanLocalVariables();

// détruit et libère l'espace mémoire utilisé de l'environnement de mysh.
void destroyEnvironment();
