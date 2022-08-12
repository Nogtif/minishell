/** Programme qui implémente fonctions utiles et réutilisables. 
 */

// on importe les librairies locales...
#include "utils.h"

/** Fonction qui prend en paramètre une chaine de caractère et un ou plusieurs séparateur(s),
 * et va fractionner la chaîne de caractère en un de sous-chaines.
 * @param str : la chaine de caractère à split.
 * @param sep : contient le/les séparateur(s).
 * @return : un tableau de chaine de caractère sans les séparateur(s).
 */
char **split(char *str, char *sep) {
  // on déclare quelques variables..
  char **res = (char **) malloc(sizeof(char *)), *tmp = NULL;
  int idx = 0;
  
  // on "explose" la chaine de caractère selon le/les spérateur(s)
  tmp = strtok(str, sep);

  while(tmp != NULL) {
    res = (char **) realloc(res, sizeof(char *) * (idx + 1));
    res[idx++] = tmp;
    tmp = strtok(NULL, sep);
  }

  // on retourne le tableau de sous-chaines.
  return res;
}

/** Fonction qui prend en paramètre un tableau de chaine de caractère et un ou plusieurs séparateur(s),
 * et va fusionner le tableau en une chaîne de caractère séparé par la chaine de séparateur.
 * @param array : la chaine de caractère à split.
 * @param sep : contient le/les séparateur(s).
 * @param count : taille du tableau.
 * @return : un tableau de chaine de caractère sans les séparateur(s).
 */
char *join(char **array, char *sep, int count) {
  // quelques variables..
  int i;
  char *res = NULL;
  size_t sizeString = 0;

  if(array == NULL || sep == NULL) return NULL;

  for(i = 0; i < count; i++) sizeString += strlen(array[i]);
  sizeString += strlen(sep) * (count);
  sizeString++;

  res = (char*)malloc(sizeString);
  res[0] = '\0';

  for(i = 0; i < count; i++) {
    strcat(res, array[i]);
    if(i < (count - 1)) strcat(res, sep);
  }

  // on retourne la chaîne complète.
  return res;
}

/** Fonction qui prend en paramètre un tableau de chaine de caractères,
 * et qui libère l'espace mémoire associé à celle-ci.
 * @param array : le tableau de chaine de caractère à libéré.
 */
void destroyArray(char **array) {
  int i;

  // on parcourt le tableau..
  // et on libère chaque chaine de caractères.
  for(i = 0; array[i]; i++) {
    free(array[i]);
  }
  // on libère le tableau.
  free(array);
}

/** Fonction qui prend en chaîne de caractère et regarde si il s'agit d'un nombre.
 * @param str : la chaîne à vérifiée.
 * @return : Vrai si c'est un nombre, faux sinon.
 */
int stringIsNumber(char *str) {
  int i = strlen(str);
  while(i--) {
    if(str[i] < '0' || str[i] > '9') {
      return 0;
    }
  }
  return 1;
}

/** Fonction qui supprime les espace en début et à la fin de la chaînée de caractère passée en paramètre.
 * @param str : la chaîne de caractère à traiter.
 * @return : la chaînée traitée.
 */
char *mytrim(char *str) {
  while(*str == ' ') str++;

  char* back = str + strlen(str);
  while((*--back) == ' ');
  *(back + 1) = '\0';

  return str; 
}

/** Fonction qui prend en paramètre une chaînée de caractère et supprime la sous-chaînes à l'instérieur.
 * @param str : la chaînée de caractère à traitrer.
 * @param sub : la sous-chaîne.
 * @return : la chaînée de caractère traitrée.
 */
char *strremove(char *str, const char *sub) {
  char *p, *q, *r;
  if(*sub && (q = r = strstr(str, sub)) != NULL) {
    size_t len = strlen(sub);
    while((r = strstr(p = r + len, sub)) != NULL) {
      while (p < r) *q++ = *p++;
    }
    while((*q++ = *p++) != '\0') continue;
  }
  return str;
}

/** Fonction qui redirige la sorie standart ou la sortie des erreur grace au paramétre
 * @param str : la chaîne de caractère.
 * @param flag : le signal.
 * @param output : le fichier de sortie.
 */
int *changeOutput(char *str, int flag, FILE *output) {
  int out, save_out, *a;

  out = open(str, flag, 0600);
  if (-1 == out) { perror(str); exit(0); }
  save_out = dup(fileno(output));
  if (-1 == dup2(out, fileno(output))) { perror("cannot redirect stdout"); exit(0); }
  a = malloc(sizeof(int)*2);
  a[0] = out;
  a[1] = save_out;
  return a;
}

/** Fonction qui prend en paramètre une chaîne de caractère et
 * supprime toutes les occurences voulues dans celle-ci.
 * @param str : la chaîne à traitée.
 * @param charToRemove : le caractère à supprimer.
 */
void removeChar(char *str, char charToRemmove) {
  int i, j;
  int len = strlen(str);

  for(i=0; i<len; i++) {
    if(str[i] == charToRemmove) {
      for(j=i; j<len; j++)  str[j] = str[j+1]; 
      len--;
      i--;
    }
  }
}

/** Fonction qui réinitialise le fichier de sortie.
 * @param out : 
 * @param output : le fichier de sortie.
 */
void resetOutPut(int out[2], FILE *output) {
  fflush(output); close(out[0]);
  dup2(out[1], fileno(output));
  close(out[1]);
}