#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include "file_handler.h"
#include "deduplication.h"

#define MD5_DIGEST_LENGTH 16  // Longueur standard d'un MD5

// Lit le fichier .backup_log et charge les entrées dans une structure chaînée
log_t read_backup_log(const char *logfile) {
    log_t logs = {NULL, NULL};

    if (logfile == NULL) {
        fprintf(stderr, "Chemin du fichier de log invalide\n");
        return logs;
    }

    FILE *file = fopen(logfile, "r");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier .backup_log");
        return logs;
    }

    char path[1024];
    char date[100];
    unsigned char md5[MD5_DIGEST_LENGTH];

    while (fscanf(file, "%1023s %99s", path, date) == 2) {
        if (fread(md5, 1, MD5_DIGEST_LENGTH, file) != MD5_DIGEST_LENGTH) {
            fprintf(stderr, "Erreur lors de la lecture du MD5\n");
            break;
        }

        log_element *elt = malloc(sizeof(log_element));
        if (elt == NULL) {
            perror("Erreur d'allocation mémoire pour un élément de log");
            break;
        }

        elt->path = strdup(path);
        if (elt->path == NULL) {
            perror("Erreur d'allocation mémoire pour le chemin");
            free(elt);
            break;
        }

        memcpy(elt->md5, md5, MD5_DIGEST_LENGTH);
        elt->date = strdup(date);
        if (elt->date == NULL) {
            perror("Erreur d'allocation mémoire pour la date");
            free(elt->path);
            free(elt);
            break;
        }

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

// Met à jour le fichier .backup_log avec les données d'une liste chaînée
void update_backup_log(const char *logfile, log_t *logs) {
    if (logs == NULL || logfile == NULL) {
        fprintf(stderr, "Logs ou chemin du fichier invalide\n");
        return;
    }

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

// Écrit un élément de log dans le fichier
void write_log_element(log_element *elt, FILE *logfile) {
    if (elt == NULL || logfile == NULL) {
        fprintf(stderr, "Élément ou fichier invalide pour l'écriture\n");
        return;
    }

    fprintf(logfile, "%s %s ", elt->path, elt->date);
    if (fwrite(elt->md5, 1, MD5_DIGEST_LENGTH, logfile) != MD5_DIGEST_LENGTH) {
        perror("Erreur lors de l'écriture du MD5");
    }
    fprintf(logfile, "\n");
}

// Liste les fichiers dans un répertoire en utilisant stat pour vérifier le type
void list_files(const char *path) {
    if (path == NULL) {
        fprintf(stderr, "Chemin invalide\n");
        return;
    }

    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Erreur d'ouverture du répertoire");
        return;
    }

    struct dirent *entry;
    char full_path[1024];
    struct stat st;

    while ((entry = readdir(dir)) != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        if (stat(full_path, &st) == -1) {
            perror("Erreur lors de l'accès aux informations du fichier");
            continue;
        }

        if (S_ISREG(st.st_mode)) {
            printf("Fichier: %s\n", entry->d_name);
        } else if (S_ISDIR(st.st_mode)) {
            printf("Répertoire: %s\n", entry->d_name);
        }
    }

    closedir(dir);
}