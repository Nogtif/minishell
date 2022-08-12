/** Programme qui implémente la commande myps. 
 */

// la librairie pour la commande myps
#include "myps.h"

/** Fonction qui prend en paramètre le nom d'un fichier, 
 * et renvoie un bufferReader de ce dernier.
 * @param filename : le nom du fichier.
 * @return: une structure bufferReader qui possède le contenu du fichier.
 */
bufferReader *createBufferReader(char *filename) {
  bufferReader *data = (bufferReader *)malloc(sizeof(bufferReader));

  // tente d'ouvrir le fichier en lecture.
  if((data->fd = open(filename, O_RDONLY)) == ERR) {
    perror("myps: Cannot open: ");
    free(data);
    exit(1);
  }
  // alloue l'espace mémoire pour le buffer.
  data->buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
  if(!(data->buffer)) {
    perror("myps: Error when allocating memory\n");
    free(data);
    exit(2);
  } else {
    for(size_t i = 0; i < BUFFER_SIZE; ++i) {
      data->buffer[i] = '\0';
    }
  }

  // tente de lire le fichier et ajouter son contenu dans le buffer.
  if(read(data->fd, data->buffer, BUFFER_SIZE) == ERR) {
    perror("myps: Cannot read: ");
    free(data->buffer);
    free(data);
    exit(3);
  }
  // renvoie le buffer.
  return data;
}

/** Fonction qui prend en paramètre bufferReader, et le supprime.
 * @param data : le buffer.
 */
void destroyBufferReader(bufferReader *data) {
  if(close(data->fd) == ERR) {
    perror("Cannot close the buffer.\n");
    exit(1);
  }
  // libère la mémoire allouée.
  free(data->buffer);
  free(data);
}

/** Fonction qui prend en paramètre les informations d'un processus 
 * et les affiches sur la sortie standard.
 * @param proc : le processus à afficher.
 */
void printProcessus(dataProc proc) {
  switch(proc.state) {
    case 'D': // ininterrompu
      printf("\033[38;5;153m");
      break;

    case 'I': // thread
      printf("\033[38;5;95m");
      break;

    case 'R': // en cours
      printf("\033[38;5;114m");
      break;

    case 'S': // interrompu
      printf("\033[38;5;215m");
      break;

    case 'T': // arrêt job signal
      printf("\033[38;5;226m");
      break;
      
    case 't': // arrêt debugger
      printf("\033[38;5;228m");
      break;
      
    case 'W': // paging
      printf("\033[38;5;35m");
      break;
      
    case 'X': // mort
      printf("\033[38;5;9m");
      break;
      
    case 'Z': // zombie
      printf("\033[38;5;243m");
      break;
    
    default:
      break;
  }

  printf("%-8s ", proc.user);
  printf("%6d\t %.1f\t%.1f  ", proc.pid, proc.cpu, proc.mem);
  printf("%7d\t%d\t%s\t%c\t", proc.vsz*4, proc.rss*4, proc.tty, proc.state);
  printf("%s\t%s\t", proc.startTime, proc.time);
  printf("%s\n\033[0m", proc.cmd);
}

/** Fonction qui va parcourir le dossier /proc du système, et va lire 
 * l'ensemble des processus dans leur dossier respectif, 
 * les stockers dans une structure dataProc et afficher les infos voulues.
 */
void readProcessus() {
  // quelques variables...
  DIR *dir; struct dirent *cur; struct tm *pTime; time_t timestamp;
  bufferReader *fileStat, *fileStatus, *fileStatm, *fileUpTime, *fileMeminfo;
  dataProc proc; char pathStat[50], pathStatus[50], pathStatm[50], pathUpTime[] = "/proc/uptime";
  int uid = -1, timeSystem = sysconf(_SC_CLK_TCK), tty_nr, tty_min, tty_maj; double uptime = 0; 
  long int memTotal = 0; long long int execTime = 0; unsigned long utime = 0, stime = 0, start_time = 0;

  // on essaye de lire le dossier /proc.
  // (renvoie une erreur et sort du programme en cas d'échec).
  if((dir = opendir("/proc")) == NULL) perror("opendir: cannot open prof folder."), exit(1);

  fileMeminfo = createBufferReader("/proc/meminfo");
  sscanf(fileMeminfo->buffer, "%*s %ld", &memTotal);

  // affiche les noms des colonnes.
  printf("\033[1;38;5;105mUSER\t    PID\t %%CPU"
    "\t%%MEM\t VSZ\tRSS\tTTY\tSTAT\tSTART\tTIME\tCOMMAND"
    "\033[0m\n"
  );

  // on lit les fichier dans le dossier.
  while((cur = readdir(dir))) {
    // si il s'agit d'un fichier de type nombre, c'est un proccessus.
    if(stringIsNumber(cur->d_name)) {
      // on récupère le bon chemin pour avoir les infos du processus.
      strcpy(pathStat, "/proc/");strcat(pathStat, cur->d_name);strcat(pathStat, "/stat");
      strcpy(pathStatus, "/proc/");strcat(pathStatus, cur->d_name);strcat(pathStatus, "/status");
      strcpy(pathStatm, "/proc/");strcat(pathStatm, cur->d_name);strcat(pathStatm, "/statm");

      // on crée le buffer du fichier pour stocker les données.
      fileStat    = createBufferReader(pathStat);
      fileStatus  = createBufferReader(pathStatus);
      fileStatm   = createBufferReader(pathStatm);
      fileUpTime  = createBufferReader(pathUpTime);

      // récupère les données depuis le fichier stat.
      sscanf(
        fileStat->buffer,
        "%d %*s %c %*s %*s %*s %d %*s %*s %*s %*s %*s %*s %lu %lu %*s %*s %*s %*s %*s %*s %lu", 
        &proc.pid, &proc.state, &tty_nr, &utime, &stime, &start_time
      );

      // solution trouvé sur stackoverflow :/
      tty_min = (tty_nr & 0xff) | ((tty_nr >> 12) & 0xfff00);
      tty_maj = (tty_nr & 0xfff00) >> 8;

      // récupère la valeur TTY.
      if(!tty_min && !tty_maj) sprintf(proc.tty, "%s", "?");
      else sprintf(proc.tty, "%s%d", "tty", tty_min);

      // récupère le nom du processus.
      sscanf(strstr(fileStatus->buffer, "Name:"), "%*s %s", proc.cmd);

      // récupère l'uid du processus et affiche son nom associé.
      sscanf(strstr(fileStatus->buffer, "Uid:"), "%*s %d", &uid);
      proc.user = (uid == -1) ? "?" : getpwuid(uid)->pw_name;

      // récupère la taille du processus et la mémoire qu'elle occupe dans la mémoire physique (RSS).
      sscanf(fileStatm->buffer, "%d %d", &proc.vsz, &proc.rss);
      
      // récupère le temps depuis que le système est run.
      sscanf(fileUpTime->buffer, "%lf %*[^\n]\n", &uptime);

      // calcul le temps CPU (+ temps exécution).
      execTime = (utime + stime) / timeSystem;
      proc.cpu = (float) (execTime * 100 / (uptime - (start_time / timeSystem)));

      // calcul de la mémoire utilisée.
      proc.mem = (proc.rss / (float)memTotal) * 100;

      // calcul du temps écoulé du processus.
      timestamp = time(NULL) - (uptime - (start_time / timeSystem));
      pTime = localtime(&timestamp);

      // récupère le temps du processus et son temps de départ.
      sprintf(proc.startTime, "%02d:%02d", pTime->tm_hour, pTime->tm_min);
      sprintf(proc.time, "%lld:%02lld", execTime / 60, execTime % 60);      

      // affiche les infos du processus..
      printProcessus(proc);

      // détruit le buffer des fichier courant.
      destroyBufferReader(fileUpTime);
      destroyBufferReader(fileStatus);
      destroyBufferReader(fileStatm);
      destroyBufferReader(fileStat);
    }
  }

  // détruit le buffer du fichier meminfo.
  destroyBufferReader(fileMeminfo);

  // renvoie une erreur et sort du programme si on n'arrive pas à fermer le dossier.
  if(closedir(dir) == -1) perror("closedir"), exit(1);
}

/** Fonction principale qui s'occupe d'exécuter la commande myps.
 * @param argc : le nombre de paramètres.
 * @param argv : tableau de chaine de caractères contenant les parmaètres.
 * @return : la valeur de retour (0) si tout c'est bien passé.
 */
int myps(int argc, char *argv[]) {
  if(argc > 1) puts("myps: Trop d'arguments.");
  else readProcessus();
  
  return 0;
}
