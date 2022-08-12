/** Programme qui implémente la commande myls. 
 */

// la librairie pour la commande myls
#include "myls.h"

// l'ensemble des options possibles définit par défault à false..
bool ls_all        = false; // -a 
bool ls_recursive  = false; // -R

/** Fonction qui prend en paramètre les droits et une chaine de caractère,
 * et va, pour chaque caractères de la chaine, associé au droit d'accès.
 * @param mode : la donnée mode du fichier.
 * @param str : la chaine de caractère à mettre à jour.
 */
void modeToString(int mode, char *str) {
  for(int i = 0; i < 10; i++) {
    // un tiret (-) par défaut.
    str[i] = '-';
    
    // si c'est un dossier, un fichier spécial de caractères...
    if(S_ISDIR(mode))       str[0] = 'd'; // dossier.
    else if(S_ISCHR(mode))  str[0] = 'c'; // fichier spécial de caractères.
    else if(S_ISBLK(mode))  str[0] = 'b'; // fichier spécial bloc.
    else if(S_ISLNK(mode))  str[0] = 'l'; // lien symbolique.
    else if(S_ISFIFO(mode)) str[0] = 'p'; // FIFO, ou un pipe.
    else if(S_ISSOCK(mode)) str[0] = 's'; // socket

    // droits pour l'utilisateur.
    if(mode & S_IRUSR) str[1] = 'r';
    if(mode & S_IWUSR) str[2] = 'w';
    if(mode & S_IXUSR) str[3] = 'x';

    // droits pour le groupe.
    if(mode & S_IRGRP) str[4] = 'r';
    if(mode & S_IWGRP) str[5] = 'w';
    if(mode & S_IXGRP) str[6] = 'x';

    // droits pour les autres.
    if(mode & S_IROTH) str[7] = 'r';
    if(mode & S_IWOTH) str[8] = 'w';
    if(mode & S_IXOTH) str[9] = 'x';


    // active la couleur associée selon le mode.
    if(str[i] == '-') printf("\033[38;5;243m");
    else if(str[i] == 'r') printf("\033[38;5;114m");
    else if(str[i] == 'w') printf("\033[38;5;215m");
    else if(str[i] == 'x') printf("\033[38;5;9m");
    else printf("\033[1;38;5;255m");

    // affiche le mode et réinitialise la couleur.
    printf("%c\033[0m", str[i]);
  }
}

/** Fonction qui prend en paramètre le nom du fichier courant et les données associées,
 * et va affiche l'ensemble des infos détaillées de ce dernier.
 * @param namefile : le nom du fichier.
 * @param data : l'objet contenant les données du fichier.
 */
void printFile(char const *namefile, struct stat data) {
  // quelques variables..
  struct tm *myTime;
  char mode[10];

  // les tailles de fichiers.
  float sizeFile, sizeKb = 1024.0f, sizeMo = sizeKb * sizeKb;
  float sizeGo = sizeMo * sizeKb, sizeTerra = sizeGo * sizeKb;

  // met à jour les accès du fichier.
  modeToString(data.st_mode, mode);

  // affiche le nb de liens, le propriétaire/groupe
  printf(
    "%3ld \033[38;5;153m %s \033[0m %s", 
    data.st_nlink, 
    getpwuid(data.st_uid)->pw_name, 
    getgrgid(data.st_gid)->gr_name
  );

  // transform size
  sizeFile = (float) data.st_size;

  // convertir et affiche la taille (B, KiB, MiB, GiB).
  if(sizeFile < sizeKb)
    printf("\033[38;5;47m%7g B  ", sizeFile);
  else if(sizeFile < sizeMo)
    printf("\033[38;5;47m%7.1f KiB", sizeFile / sizeKb);
  else if(sizeFile < sizeGo)
    printf("\033[38;5;216m%7.1f MiB", sizeFile / sizeMo);
  else if(sizeFile < sizeTerra)
    printf("\033[38;5;9m%7.1f GiB", sizeFile / sizeGo);

  // renvoie une erreur et sort du programme si on n'arrive pas récupérer la date locale.
  if(!(myTime = localtime(&data.st_mtime))) perror("localtime"), exit(1);

  // affiche la date du fichier.
  printf("\033[38;5;243m%d-%02d-%02d %02d:%02d \033[0m ", 
    myTime->tm_year + 1900, myTime->tm_mon, myTime->tm_mday, 
    myTime->tm_hour, myTime->tm_min
  );

  // affiche le nom du fichier avec un code couleur associé à son type.
  if(S_ISDIR(data.st_mode)) printf("\033[1;38;5;105m%s\033[0m", namefile);
  else if(S_ISCHR(data.st_mode) || S_ISBLK(data.st_mode)) printf("\033[38;5;190m%s", namefile);
  else if(S_ISLNK(data.st_mode)) printf("\033[1;38;5;123m%s", namefile);
  else if(S_ISFIFO(data.st_mode)) printf("\033[38;5;172m%s", namefile);
  else if(S_ISSOCK(data.st_mode)) printf("\033[1;38;5;170m%s", namefile);
  else if(namefile[0] == '.') printf("\033[38;5;247m%s", namefile);
  else printf("%s", namefile); 
  
  printf("\033[0m\n"); // petit saut de ligne qui fait plaisir :)
}

/** Fonction qui va lister de manière détaillé, le contenu du répertoire fournit en paramètre.
 * L'affichage varie selon les options qui sont actives ou non. 
 * @param filename : le nom du dossier.
 */
void readDirectory(const char *filename) {
  // quelques variables...
  DIR *dir = opendir(filename);
  struct dirent *cur;
  struct stat data;
  char *buffer = NULL, **memo = NULL;
  unsigned int i, sizeMemo = 0, sizeBuffer = 0;

  // si ce n'est pas un dossier..
  if(dir == NULL) {
    // on essaye de lire le fichier (et on l'affiche si on y arrive).
    if(!stat(filename, &data)) {
      printFile(filename, data);
      return;
    }
    // sinon on renvoie une erreur et on sort du programme.
    else {
      perror("myls: opendir"); 
      return;
    }
  }

  // si c'est récursive, on affiche le nom du répertoire courant.
  if(ls_recursive) printf("\033[1;38;5;105m%s: \033[0m\n\n", filename);

  // lit le répertoire.
  while((cur = readdir(dir))) {
    // si ce n'est pas un fichier caché, ou que l'option -a est actif.
    if((cur->d_name[0] == '.' && ls_all) || cur->d_name[0] != '.') {
      // initialise le buffer.
      sizeBuffer = 10 + strlen(filename) + strlen(cur->d_name);
      buffer = malloc(sizeof(char) * sizeBuffer);
      assert(buffer);

      sprintf(buffer, "%s/%s", filename, cur->d_name);

      // vérifie qu'on à bien la permission de lire le fichier.
      if(lstat(buffer, &data) == -1) {
        perror("myls: Permission non-autorisé"), 
        free(buffer); 
        break;
      }

      // affiche le fichier courant.
      printFile(cur->d_name, data);

      // on stocke en mémoire si on à l'option récursive activée.
      if(ls_recursive && S_ISDIR(data.st_mode)) {
        if(!ls_all || (ls_all && strcmp(cur->d_name, ".") && strcmp(cur->d_name, ".."))) {
          memo = (char **)realloc(memo, sizeof(char *) * (sizeMemo + 1));
          memo[sizeMemo++] = buffer;
        } else free(buffer);
      } else free(buffer);
    }
  }

  /* parcours de les fichiers en file d'attente ! */
  for (i = 0; i < sizeMemo; i++) {
    printf("\n");
    readDirectory(memo[i]);
    free(memo[i]);
  }

  // libère la mémoire..
  free(memo);
  
  // renvoie une erreur et sort du programme si on n'arrive pas à fermer le dossier.
  if(closedir(dir) == -1) perror("closedir"), exit(1);
}

/** Fonction qui affiche un message d'aide sur la sortie standard.
 */ 
void mylsHelp() {
  printf("MYLS: Besoin d'aide ?\n"
  "\n\033[1;32mNOM\033[0m\n\tmyls - Affiche le contenu détaillé d'un répertoire.\n"
  "\n\033[1;32mSYNOPSIS\033[0m\n\t\033[1mmyls [OPTION]... [FILE]...\033[0m"
  "\n\tOptions POSIX: \033[1m[aR]\033[0m\n"
  "\n\033[1;32mDESCRIPTION\033[0m\n"
  "\tEn plus du nom, affiche le type de fichier, les autorisations d'accès, le nombre de\n"
  "\tliens physiques, nom du propriétaire et du groupe, octets et date de la dernière\n"
  "\tmodification. Les options peuvent être combinés. Si aucune entrée à la liste n'est\n"
  "\tpassée en paramètre.\n"
  "\n\033[1;32mPOSIX OPTIONS\033[0m\n"
  "\t\033[1m-a\033[0m\t Affiche tous les fichiers dans les répertoires, y compris les fichiers\n"
  "\t\t commençant par un `.`.\n"
  "\n\t\033[1m-R\033[0m\t Affiche récursivement le contenu des sous-répertoires.\n\n");
}

/** Fonction principale qui s'occupe d'exécuter la commande myls.
 * @param argc : le nombre de paramètres.
 * @param argv : tableau de chaine de caractères contenant les parmaètres.
 * @return : la valeur de retour (0) si tout c'est bien passé.
 */
int myls(int argc, char *argv[]) {
  // quelques variables..
  int ind, validOpts = 0;
  struct stat data;
  char *params = join(argv, " ", argc);
  char *options = strchr(params, '-');
  
  // met à jour les status des options.
  if(options != NULL) {
    validOpts = 0;
    if(!strcmp(options, "--help")) mylsHelp(), free(params), exit(2);
    while(*++options && *options != ' ') {
      if(*options == 'a') ls_all = true;
      else if(*options == 'R') ls_recursive = true;
      else printf("myls: %c: Option inconnue\n", *options), free(params), exit(2);
      validOpts = 1;
    }
  }

  // si il n'y aucun paramètre, on affiche celui courant.
  if(argc == 1 || (argc == 2 && validOpts)) readDirectory(".");

  // sinon...
  else for(ind = 1; ind < argc; ind++) {

    // si ce n'est pas une option.
    if(*argv[ind] != '-' || !validOpts) {

      // vérifie les droits d'accès au dossier.
      if(stat(argv[ind], &data) == -1) perror(argv[ind]);

      // affiche le nom du répertore si c'est un dossier.
      if(S_ISDIR(data.st_mode) && !ls_recursive) printf("\033[1;38;5;105m%s: \033[0m\n\n", argv[ind]);

      // parcours le dossier.
      readDirectory(argv[ind]);
      printf("\n");
    }
  }

  // libère la mémoire utilisée pour stocker les paramètres..
  free(params);
  return 0;
}