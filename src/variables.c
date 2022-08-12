/** Programme qui implémente les variables globales et locales. 
 */

// la librairie des variables.
#include "variables.h"

// Variables globales..
List *locals;

struct sembuf UP = {0, +1, 0};
struct sembuf DW = {0, -1, 0};

/** Fonction qui initialise l'environnement du shell.
 */
int initEnvironment() {
  locals = NULL;
  return 1;
}

/** Méthode qui prend en paramètre le nom d'une variable,
 * et parcours la liste des variables locales pour la renvoyer.
 * @param name : le nom de la variable. 
 * @return : la variable si elle est trouvée, NULL sinon.
 */
char *getLocalVariable(char *name) {
  char *tmp = mytrim(name);
  List *prec = locals;

  if(prec == NULL || tmp == NULL || *tmp != '$') return NULL;

  while(prec != NULL) {
    if(!strcmp(prec->var->name, tmp)) {
      return prec->var->value;
    }
    prec = prec->next;
  }

  return NULL;
}

/** Fonction qui affiche les variables locales existantes.
 */
void printLocalVariables() {
  List *prec = locals;

  // parcours la liste et affiche les variables.
  while(prec != NULL) {
    printf("%s = %s|\n", prec->var->name, prec->var->value);
    prec = prec->next;
  }
}

/** Fonction qui prend en paramètre une variable, ou suite de variable,
 * et les affichent. On privilégie ici les variables locales en priorité.
 */
char *parseVariable(char *var) {
  char *buff = malloc(sizeof(char) * BUFFERSIZE), varVal[BUFFERSIZE], *tmp = var;
  int i, j, sizeVar = 0, sizeBuff = 0, getVar = 0;

  for(i = 0; i <= strlen(tmp); i++) {
    if(tmp[i] == '$') getVar = 1;
    else if(getVar && (!isalnum(tmp[i]) || i == strlen(tmp))) {
      varVal[sizeVar] = '\0';
      sprintf(varVal, "%s", getLocalVariable(varVal));
      for(j = 0; j < strlen(varVal);j++) buff[sizeBuff++] = varVal[j];
      sizeVar = 0; getVar = 0;
    }

    if(getVar) varVal[sizeVar++] = tmp[i];
    else buff[sizeBuff++] = tmp[i];
  }
  buff[sizeBuff] = '\0';
  buff = realloc(buff, sizeof(char) * (sizeBuff+1));

  return buff;
}

/** Fonction qui prend en paramètre une séquence de variable,
 * et affiche cette dernière en remplacer les variables donnée par leur valeur.
 * @param var : la séquence de variable à traduire.
 */
int echoVariable(char *var) {
  char *res = parseVariable(var);
  printf("%s\n", res);
  free(res);
  return 0;
}

/** Fonction qui prend en paramètre le nom d'une variable, 
 * et vérifie si ce dernier est valide.
 * @param name : le nom de la variable à vérifiée.
 * @return : vrai si c'est valide, faux sinon.
 */
int validNameVariable(char *name) {
  int i = 0;

  if(name == NULL) return 0;

  while(name[i] != '\0') {
    if(name[i] == ' ') return 0;
    i++;
  }

  return 1;
}

/** Fonction qui prend en paramètre un nom et une valeur qui lui est associée,
 * et va créer et ajoute la variable à la liste de celles déjà existantes.
 * @param newVar : la sequence de la variable à ajoutée.
 * @return : 1 si tout c'est bien passé, 0 sinon.
 */
int setLocalVariable(char *var) {
  // vérifie que les informations en paramètre sont valides.
  if(var == NULL) return 0;

  // quelques variables...
  char *tmp = var;
  char buff[strlen(var)],*trim = NULL;
  int sizeBuff = 0;
  Variable *newVar = (Variable *)malloc(sizeof(Variable));

  buff[sizeBuff++] = '$';
  while(*tmp == ' ') tmp++;
  while(*tmp != '=') buff[sizeBuff++] = *tmp++;
  if(*tmp == '\0') return 0;
  buff[sizeBuff] = '\0';

  trim = mytrim(buff);

  if(!validNameVariable(trim)) {
    free(newVar);
    return 0;
  }
  
  newVar->name = (char *)malloc(sizeof(char) * (sizeBuff+1));
  strcpy(newVar->name, trim);

  sizeBuff = 0; tmp++;
  while(*tmp == ' ') tmp++; 
  newVar->value = parseVariable(tmp);

  List *l = malloc(sizeof(List));
  l->var = newVar;
  l->next = NULL;

  List *prec = locals;
  
  if(locals != NULL) {
    if(!strcmp(locals->var->name, newVar->name)) {
      strcpy(locals->var->value, newVar->value);
      free(newVar->name), free(newVar->value);
      free(l->var), free(l);
      return 1;
    } 
    else if(locals->next == NULL) {
      l->next = NULL;
      locals->next = l;
      return 1;
    } else {
      while(prec != NULL) {
        if(!strcmp(prec->var->name, newVar->name)) {
          strcpy(prec->var->value, newVar->value);
          free(newVar->name), free(newVar->value);
          free(l->var), free(l);
          return 1;
        }
        else if(prec->next == NULL) {
          l->next = NULL;
          prec->next = l;
          return 1;
        }
        prec = prec->next;
      }
    }

  } else {
    l->next = NULL;
    locals = l;
    return 1;
  }

  return 0;
}

/** Fonction qui prend en paramètre le nom d'une variable locale 
 * et la supprime de la liste.
 * @param name le nom de la variable à supprimer.
 * @return : 1 si la variable a bien été supprimée, 0 sinon.
 */
int unsetLocalVariable(char *name) {
  char tmp[strlen(name)]; int i = 0;
  while(*name != '\0') {
    if(*name != ' ') tmp[i++] = *name;
    name++;
  } tmp[i] = '\0';

  if(locals == NULL || tmp == NULL || *tmp != '$') return 0;

  List *prec = locals, *curr = locals;

  while(prec != NULL) {
    if(!strcmp(prec->var->name, tmp)) {
      if(prec == locals) {
        if(prec->next != NULL) {
          locals = prec->next;
          free(prec->var->name), free(prec->var->value);
          free(prec->var), free(prec);
        } else cleanLocalVariables();
      } else {
        curr->next = prec->next;
        free(prec->var->name), free(prec->var->value);
        free(prec->var), free(prec);
      }
      return 1;
    }
    curr = prec;
    prec = prec->next;
  }

  return 0;
}

/** Fonction qui supprime l'ensemble des variables globales 
 * et libère l'espace mémoire qui leur était attribués. 
 */
void cleanLocalVariables() {
  if(locals == NULL) return;

  List *tmp;
  while(locals != NULL) {
    tmp = locals;
    locals = locals->next;
    free(tmp->var->name);
    free(tmp->var->value);
    free(tmp->var);
    free(tmp);
  }
}

/** Fonction qui supprime l'environnement de mysh 
 * en libèrant l'espace mémoire utilisé pour les variables locales. 
 */
void destroyEnvironment() {
  cleanLocalVariables();
}
