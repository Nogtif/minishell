// on importe les librairies locales...
#include "myshell.h"

// variables globales.
Command listCommands[NB_COMMANDS];
myProc execprocs[MAX_PROCS];
int lastprocc = -1, laststatus = -1, nbprocessus = 0;
const char * errormsg[] = {
  "No error", // 0
  RED("Impossible to fork procces"), // 1
  RED("Exec failed") //2
};

/** Fonction qui affiche le message d'accueil du Mini-shell.
 * (avec une magnifique copie du pingouin linux).
 */
void welcomeMessage() {
  printf(
    "    .--.      \n"
    "   |o_o |     \t\033[1;38;5;105mProjet Mini-shell\033[0m\n"
    "   |:_/ |     \t----------------------\n"
    "  //   \\ \\  \t\033[1;38;5;105mRéalisé par: \033[0mQuentin Carpentier et Paul-joseph Krogulec\n"
    " (|     | )   \t\033[1;38;5;105mAnnée: \033[0m2021 - 2022\n"
    "/'\\_   _/`\\ \n"
    "\\___)=(___/\n"
    "\n\n"
  );
}

/** Fonction qui affiche le répertoire courant dans le shell.
 */
void printPath() {
  // les variables..
  char cwd[SIZE], homePath[SIZE], *user = getenv("LOGNAME");
  int lenHomeDirectory = 0;

  // définit la home directory de l'utilisateur courant
  sprintf(homePath, "/home/%s", user);
  // et récupère le chemin courant.
  getcwd(cwd, SIZE);

  // si dans le chemin, la home directory est présente, on récupère sa longueur.
  if(strstr(cwd, homePath)) lenHomeDirectory = strlen(homePath);

  // on affiche le répertoire courant en fonction de la home directory.
  printf(
    "\n\033[1;38;5;15m[MyShell] \033[1;38;5;215m%s%.*s\n\033[38;5;105m> \033[0m", 
    lenHomeDirectory ? "~" : "", (int)strlen(cwd), &cwd[lenHomeDirectory]
  );
}

/** Fonction qui effectue la commande interne status,
 * qui affiche pour le dernier processus (xxxx) exécuté en premier plan (en foreground).
 */
void mystatus(){
  // si il n'y a pas d'ancienne commande.
  if(lastprocc == ERR) {
    puts("Dernière commande inexistante");
    return;
  }
  
  if(WIFEXITED(laststatus)) {
    printf("%d ", lastprocc);
    printf(GREEN("terminé avec comme code de retour = ")), printf("%d \n", laststatus);
  } else if (WIFSIGNALED(laststatus)){
    printf("%d ", lastprocc);
    printf(RED("terminé anormalement dans le cas d'une erreur dans la commande.\n"));
  }
}

/** Fonction affiche la liste des processus en arrière plan.
 */
void myjobs() {
  int i;

  if(nbprocessus == 0) {
    printf("Pas de processus en cours d'exécution en arrière plan.\n");
    return;
  }

  // on commence à 1, le processus à l'indice 0 est celui en foreground.
  for(i = 1; i < nbprocessus; i++) {
    if(execprocs[i].pid) {
      printf("[%d] %d", i, execprocs[i].pid);
      printf("%s", execprocs[i].isRunning ? " En cours d’exécution" : "Stoppé");
      printf("%s", execprocs[i].cmd.name);
    }
  }
}


/** Fonction qui initialise et remplie le tableau des commandes internes,
 * en associant avec leur pointeur de fonction.
 */
void initListCommands() {
  Command tmp;

  tmp.name = "mycd";
  tmp.fct = &mycd;
  listCommands[0] = tmp;

  tmp.name = "myls";
  tmp.fct = &myls;
  listCommands[1] = tmp;

  tmp.name = "myps";
  tmp.fct = &myps;
  listCommands[2] = tmp;
}

/** Fonction qui prend en paramètre le nom d'une commande possible,
 * et renvoie l'indice associé cette dernière si elle fait partie de
 * la liste des commandes interne de myShell.
 * @param cmd : le nom de la commande.
 * @return : l'indice de la commande dans la liste de commandes internes. 
 * (-1 si elle n'est pas dedans).
 */
int isInternalCommand(char *cmd) {
  int i = 0;

  for(i = 0; i < NB_COMMANDS;i++) {
    if(!strcmp(listCommands[i].name, cmd)) {
      return i;
    }
  }
  return -1;
}

/** Fonction qui prend en paramètre une commande et l'exécute.
 * Si c'est une commande interne, on appelle sa méthode associée depuis 
 * son pointeur de fonction, sinon on l'exécute depuis la commande système.
 * @param cmd : la commande à exécutée.
 */
void execCommand(Command cmd, int argc, char *argv[]) {
  // quelques variables..
  int internalCmd = isInternalCommand(cmd.name);
  char *tmp = NULL;

  // si il s'agit d'une commande interne.
  if(internalCmd != -1) {

    // on appelle lacommande mycd.
    if(internalCmd == 0) ((*listCommands[0].fct)(argc, argv));

    // on appelle la commande myls.
    else if(internalCmd == 1) ((*listCommands[1].fct)(argc, argv));

    // on appelle la commande myps.
    else if(internalCmd == 2) ((*listCommands[2].fct)(argc, argv));

  } else {
    // ajouter une variable locale.
    if(!strcmp(cmd.name, "set")) {
      // concat le tableau d'arguments en chaîne de caractère.
      tmp = join(argv, " ", argc);
      setLocalVariable(tmp + 3);
      free(tmp);
    }

    // affiche une variable.
    // (on privilégie la variable locale si elle existe).    
    else if(!strcmp(cmd.name, "echo"))  {
      // concat le tableau d'arguments en chaîne de caractère.
      tmp = join(argv, " ", argc);
      echoVariable(tmp + 4);
      free(tmp);
    } 
    
    // supprimer une variable locale.  
    else if(!strcmp(cmd.name, "unset"))  {
      // concat le tableau d'arguments en chaîne de caractère.
      tmp = join(argv, " ", argc);
      unsetLocalVariable(tmp + 5);
      free(tmp);
    }

    // si il s'agit d'une commande externe.
    else execvp(cmd.name, argv);
  }
}

/** Fonction qui execute une commande.
 * @param cmd : la commande.
 * @param tabcms : le tableau de commande.
 * @param i : l'indice de la commande dans le tableau.
 */
int runCommand(Command cmd, char *argv[], int argc) {
  // les variables..
  int status, ind = 0, n, idx, nbwild = 0; pid_t pid;
  glob_t paths; char *arraycmd[SIZECMD], *arraywild[SIZECMD];

  // initialise le tableau par defaut
  for(n = 0; n < SIZECMD; n++) {
    arraycmd[n] = NULL;
    arraywild[n] = NULL;
  }

  // initialise le nom et met à null le dernier elem+1.
  cmd.name = argv[0];
  argv[argc] = NULL;

  // si la commande n'est pas vide.
  if(argc) {

    // affiche le statut de la dernière commande.
    if(!strcmp(cmd.name, "status")) {
      mystatus();
      return 0;
    }

    // affiche la liste des processus en arrière plan.
    else if(!strcmp(cmd.name, "myjobs")) {
      myjobs();
      return 0;
    }

    // on vérifie si il s'agit de la commande exit.
    else if(!strcmp(cmd.name, "exit")) {
      destroyEnvironment();
      exit(0);
    }

    // On exécute la commande sans fork quand il s'agit des commandes mycd
    // et celles par rapport aux variables.
    else if(!strcmp(cmd.name, "mycd") || !strcmp(cmd.name, "set") 
      || !strcmp(cmd.name, "echo") || !strcmp(cmd.name, "unset")) {
      execCommand(cmd, argc, argv);
      return 0;
    }

    // on fork.
    if((pid = fork()) == ERR) fatalsyserror(1);
    lastprocc = pid;

    execprocs[0].cmd = cmd;
    execprocs[0].pid = pid;

    // si c'est le fils.
    if(!pid) {
      execprocs[0].isRunning = 1;

      // on parcours chaque élément de du tableau d'aguments.
      for(n = 0; n < argc; n++) {
        // on copie chaque argument dans le nouveau tableau.
        arraycmd[ind] = argv[n];
        // si la chaîne contient un élément succeptible d'être de type wildcard, on le traite.
        if(strstr(argv[n], "*") || strstr(argv[n], "?") || (strstr(argv[n], "[") && strstr(argv[n], "]"))) {
          paths.gl_pathv = NULL; paths.gl_pathc = 0; paths.gl_offs = 0;

          // on utilise la fonction glob pour avoir l'ensemble des résultats.
          if(glob(argv[n], 0, NULL, &paths) == 0) {
            // pour chaque résultat, on l'ajout dans le nouveau tableau.
            for(idx = 0; idx < paths.gl_pathc; idx++) {
              arraywild[nbwild] = (char *)malloc(sizeof(char) * (strlen(paths.gl_pathv[idx]) + 1));
              strcpy(arraywild[nbwild++], paths.gl_pathv[idx]);
              arraycmd[ind++] = arraywild[nbwild-1];
            }
            // libère la mémmoire dans paths pour les wildcards.
            globfree(&paths);

          // sinon on renvoie une erreur de glob.
          } else puts("glob: erreur");
        }
        if(ind == n) ind++; // on incrèmente ind si cela n'a pas déjà été fait.
      }      

      // on exécute la commande.
      execCommand(cmd, ind, arraycmd);

      // libère la mémoire utilisé pour les wildcards.
      for(n = 0; n < SIZECMD; n++) free(arraywild[n]);

      // on sort du processus une fois la commande exécutée.
      exit(0);
    }
    // si c'est le père..
    else {
      // on attends le père et on envoie la réponse.
      waitpid(pid, &status, 0);
      if(WIFEXITED(status)) {
        if((status = WEXITSTATUS(status)) != FAILEDEXEC) {
          laststatus = status;
          mystatus();
        }
      }
    }
  }
  return status;
}

/** Fonction qui parse la commande donnée en paramètre et la lance.
 * @param command : la commande à parser.
 */
void createCommand(char *command) {
  // on verifie que la commande est valide.
  if(command == NULL || !strcmp(command, "")) return;

  // déclaration des variables..
  Command cmd;
  char *tabcmd[SIZE], *s = "\0";
  int i, status, *a = NULL, *a1 = NULL, *a2 = NULL;

  for(s = command; isspace(*s); s++);
  for(i = 0; *s; i++) {
    if(*s == ';') {
      runCommand(cmd,tabcmd,i);
      memset(tabcmd,0,i);
      i = -1;
      s++;
    } else if(*s == '|' && *(s+1) == '|') {
      status = runCommand(cmd,tabcmd,i);
      memset(tabcmd,0,i);
      i = -1;
      s++;
      wait(&status);
      if(status != 0 && getpid() == 0) {
        puts("\n\n");
        s++;
        createCommand(s);
      } else {
        if (status == 0) {
          while(isspace(*s)) s++;
          while(!isspace(*s)) s++;
          *s++ = '\0';
          while(isspace(*s)) s++;
        }
      }
    } else if(*s == '&' && *(s+1) == '&') {
      status = runCommand(cmd,tabcmd,i);
      memset(tabcmd,0,i);
      i = -1;
      s++;
      wait(&status);
      if(status == 0 && getpid() == 0) {
        puts("\n\n");
        s++;
        createCommand(s);
      } else {
        if (status != 0) {
          while(isspace(*s)) s++;
          while(!isspace(*s)) s++;
          *s++ = '\0';
          while(isspace(*s)) s++;
        }
      }
    } else if(*s == '>') {
      if (*(s+1) == '>') {
        while(isspace(*s)) s++;
        while(!isspace(*s)) s++;
        *s++ = '\0';
        while(isspace(*s)) s++;
        removeChar(s,'\'');
        removeChar(s,'\n');
        removeChar(s,'$');
        if (*(s+2) == '&') { 
          a1 = changeOutput(s,O_RDWR|O_CREAT|O_APPEND,stdout);
          a2 = changeOutput(s,O_RDWR|O_CREAT|O_APPEND,stderr);
          runCommand(cmd,tabcmd,i);
          resetOutPut(a2,stderr);
          resetOutPut(a1,stdout);
          free(a1), free(a2);
        } else {
          a = changeOutput(s,O_RDWR|O_CREAT|O_APPEND,stdout);
          runCommand(cmd,tabcmd,i);
          resetOutPut(a,stdout);
          free(a);
        }
      } else {
        while(isspace(*s)) s++;
        while(!isspace(*s)) s++;
        *s++ = '\0';
        while(isspace(*s)) s++;
        removeChar(s,'\'');
        removeChar(s,'\n');
        removeChar(s,'$');
        if(*(s+1) == '&') { 
          a1 = changeOutput(s,O_RDWR|O_CREAT|O_TRUNC,stdout);
          a2 = changeOutput(s,O_RDWR|O_CREAT|O_APPEND,stderr);
          runCommand(cmd,tabcmd,i);
          resetOutPut(a2,stderr);
          resetOutPut(a1,stdout);
          free(a1), free(a2);
        } else {
          a = changeOutput(s,O_RDWR|O_CREAT|O_TRUNC,stdout);
          runCommand(cmd,tabcmd,i);
          resetOutPut(a, stdout);
          free(a);
        }
      }
    } else if(*s == '2' && *(s+1) == '>') {
      while(isspace(*s)) s++;
      while(!isspace(*s)) s++;
      *s++ = '\0';
      while(isspace(*s)) s++;
        removeChar(s,'\'');
        removeChar(s,'\n');
        removeChar(s,'$');
      if (*(s+2) == '>') {
        a = changeOutput(s,O_RDWR|O_CREAT|O_APPEND,stderr);
        runCommand(cmd,tabcmd,i);
        resetOutPut(a,stderr);
        free(a);

      } else {
        a = changeOutput(s,O_RDWR|O_CREAT|O_TRUNC,stderr);
        runCommand(cmd,tabcmd,i);
        resetOutPut(a,stderr);
        free(a);
      }
    } else if(*s == '<' ) {
      while(isspace(*s)) s++;
      while(!isspace(*s)) s++;
      *s++ = '\0';
      while(isspace(*s)) s++;
      removeChar(s,'\'');
      removeChar(s,'\n');
      removeChar(s,'$');
      a = changeOutput(s,O_RDONLY,stdin);
      runCommand(cmd,tabcmd,i);
      resetOutPut(a,stdin);
      free(a);
    } else tabcmd[i] = s;
    while(!isspace(*s)) s++;
    *s++ = '\0';
    while(isspace(*s)) s++;
  }
  runCommand(cmd,tabcmd,i);
}

/** Fonction qui s'occupe de l'événement du Ctrl-c.
 * Celui de demander de quitter le shell et de libérer la mémoire utilisée.
 * @param sig : le signal réçu.
 */
void ctrlC(int sig) {
  char res;

  if(execprocs[0].pid && execprocs[0].isRunning) {
    kill(execprocs[0].pid, SIGKILL);
  }

  printf("\nVoulez-vous vraiment quitter ce magnifique Shell ? [o/n]: ");
  res = getchar();

  if(res == 'o' || res == 'O') {
    destroyEnvironment();
    exit(0);
  }
}

/** Fonction qui initialise et génére un prompt shell.
 * @param void : rien.
 * @return : valeur de retour par default.
 */
int mysh(void) {
  // on définit les variables.
  char command[SIZECMD];
  
  // affiche le message d'accueil du shelL.
  welcomeMessage();
  
  // initialise l'environnement du shell (variables globales et locales).
  initEnvironment();

  // initialise les commandes internes.
  initListCommands();

  // signal pour le ctrl-c
  if(signal(SIGINT, ctrlC) == SIG_ERR) fatalsyserror(1);

  // signal pour le ctrl-z
  // if(signal(SIGTSTP, ctrlZ) == SIG_ERR) fatalsyserror(1);

  // prompt..
  for(;;) {
    // affiche le le répertoire courant.
    printPath();

    if(!fgets(command, SIZECMD-1, stdin)) exit(FAILEDEXEC);
    else createCommand(command);
  }
  destroyEnvironment();
  exit(0);
}