#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include "file_handler.h"
#include "deduplication.h"

// Fonction permettant de lire un élément du fichier .backup_log
log_t read_backup_log(const char *logfile) {
    log_t logs = { NULL, NULL };
    FILE *file = fopen(logfile, "r");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier .backup_log");
        return logs;
    }

    char path[1024];
    char date[100];
    unsigned char md5[MD5_DIGEST_LENGTH];

    while (fscanf(file, "%s %s ", path, date) == 2) {
        if (fread(md5, 1, MD5_DIGEST_LENGTH, file) != MD5_DIGEST_LENGTH) {
            break;
        }
        log_element *elt = malloc(sizeof(log_element));
        elt->path = strdup(path);
        memcpy(elt->md5, md5, MD5_DIGEST_LENGTH);
        elt->date = strdup(date);
        elt->next = logs.head;
        if (logs.head != NULL) {
            logs.head->prev = elt;
        }
        logs.head = elt;
        if (logs.tail == NULL) {
            logs.tail = elt;
        }
    }
    fclose(file);
    return logs;
}

// Fonction permettant de mettre à jour une ligne du fichier .backup_log
void update_backup_log(const char *logfile, log_t *logs) {
    FILE *file = fopen(logfile, "w");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier .backup_log");
        return;
    }

    log_element *current = logs->head;
    while (current != NULL) {
        write_log_element(current, file);
        current = current->next;
    }
    fclose(file);
}

// Fonction pour écrire un élément log de la liste chaînée log_element dans le fichier .backup_log
void write_log_element(log_element *elt, FILE *logfile) {
    fprintf(logfile, "%s %s ", elt->path, elt->date);
    fwrite(elt->md5, 1, MD5_DIGEST_LENGTH, logfile);
    fprintf(logfile, "\n");
}

// Fonction permettant de lister les fichiers présents dans un répertoire
void list_files(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Erreur d'ouverture du répertoire");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            printf("Fichier: %s\n", entry->d_name);
        }
    }
    closedir(dir);
}

