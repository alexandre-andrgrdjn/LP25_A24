#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <stdio.h>
#include <openssl/md5.h>

// Structure pour une ligne du fichier log
typedef struct log_element{
    const char *path; // Chemin du fichier/dossier
    unsigned char md5[MD5_DIGEST_LENGTH]; // MD5 du fichier dédupliqué
    char *date; // Date de dernière modification
    struct log_element *next; // Pointeur vers l'élément suivant
    struct log_element *prev; // Pointeur vers l'élément précédent
} log_element;

// Structure pour une liste de log représentant le contenu du fichier backup_log
typedef struct {
    log_element *head; // Début de la liste de log 
    log_element *tail; // Fin de la liste de log
} log_t;

// Fonction pour lire le fichier de log de sauvegarde
log_t read_backup_log(const char *logfile);

// Fonction pour mettre à jour le fichier de log de sauvegarde
void update_backup_log(const char *logfile, log_t *logs);

// Fonction pour écrire un élément de log dans le fichier
void write_log_element(log_element *elt, FILE *logfile);

// Fonction pour lister les fichiers dans un répertoire
void list_files(const char *path);

// Fonction pour copier un fichier d'une source vers une destination
void copy_file(const char *src, const char *dest);

#endif // FILE_HANDLER_H
