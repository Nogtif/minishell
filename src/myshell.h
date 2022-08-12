// on importe quelques librairies...
#include <ctype.h>
#include <errno.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// librairies locales..
#include "commands/mycd.h"
#include "commands/myls.h"
#include "commands/myps.h"
#include "variables.h"
#include "utils.h"

// quelques constantes...
#define ERR -1
#define SIZE 1024
#define SIZECMD 4096
#define FAILEDEXEC 127
#define NB_COMMANDS 3
#define MAX_PROCS 255

// on définit quelques macros...
#define syserror(n) perror(errormsg[n])
#define fatalsyserror(n) syserror(n), exit(n)

/** Structure qui modèlise une commande interne avec son nom,
 * savoir s'il s'agit d'une commande en exécutée en background,
 * et son pointeur de fonction qui lui est associé.
 */
typedef struct {
  char *name;
  int isbackground;
  int (*fct)(int, char **);
  int status;
} Command;

/** Structure qui modèlise un processus en cours avec,
 * sa commande, son pid et son état.
 * (si il est en cours d'éxecution ou stoppé).
 */
typedef struct {
  Command cmd;
  pid_t pid;
  int isRunning;
} myProc;

// initialise et génére un prompt shell.
int mysh(void);