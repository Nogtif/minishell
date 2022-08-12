// les includes qui vont bien...
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>

// on définit quelques macros de couleurs...
#define RED(m)    "\033[0;31m"m"\033[0m"
#define GREEN(m)  "\033[0;32m"m"\033[0m"
#define BLUE(m)   "\033[0;34m"m"\033[0m"

#define BRED(m)     printf("\033[1;31m%s\033[0m", m)
#define BGREEN(m)   printf("\033[1;32m%s\033[0m", m)
#define BYELLOW(m)  printf("\033[1;33m%s\033[0m", m)
#define BBLUE(m)    printf("\033[1;34m%s\033[0m", m)
#define BPURPLE(m)  printf("\033[1;35m%s\033[0m", m)
#define BCYAN(m)    printf("\033[1;36m%s\033[0m", m)

// les constantes globales..
#define ERR -1
#define MAX_SIZE 1021
#define BUFFERSIZE 1024

#define isDigit(a) (((a) >= '0') && ((a) <= '9'))

// fractionne une chaîne de caractère en un tableau de sous-chaînes 
char **split(char *str, char *sep);

// fusionne un tableau de sous-chaînes de caractères en une seule chaîne.
char *join(char **array, char *sep, int count);

// libère l'espace mémoire d'un tableau de caractère.
void destroyArray(char **array);

// regarde si la chaine de caractère passée en paramètre est un nombre.
int stringIsNumber(char *str);

// supprime les espace au début et à la fin de la chaîne de caractère.
char *mytrim(char *s);

//
char *strremove(char *str, const char *sub);

// 
int *changeOutput(char *str, int flag, FILE *output);

// retire toutes les occurences d'un caractère dans la chaîne.
void removeChar(char *str, char charToRemmove);

// 
void resetOutPut(int out[2], FILE *output);