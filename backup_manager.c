#include "backup_manager.h"
#include "deduplication.h"
#include "file_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>

// Fonction permettant d'enregistrer dans fichier le tableau de chunk dédupliqué
void write_backup_file(const char *output_filename, Chunk *chunks, int chunk_count) {
    FILE *output_file = fopen(output_filename, "wb");
    if (output_file == NULL) {
        perror("Erreur d'ouverture du fichier de sauvegarde");
        return;
    }

    // Écrire chaque chunk dans le fichier
    for (int i = 0; i < chunk_count; i++) {
        fwrite(chunks[i].data, 1, CHUNK_SIZE, output_file);
    }

    fclose(output_file);
}

// Fonction implémentant la logique pour la sauvegarde d'un fichier
void backup_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier à sauvegarder");
        return;
    }

    // Table de hachage pour les chunks
    Chunk chunks[1000];
    Md5Entry hash_table[HASH_TABLE_SIZE] = {0};  // Table de hachage vide
    int chunk_count = 0;

    // Dédupliquer le fichier
    deduplicate_file(file, chunks, hash_table);
    fclose(file);

    // Écrire les chunks dédupliqués dans un fichier de sauvegarde
    write_backup_file("backup_data.sav", chunks, chunk_count);
}

// Fonction permettant la restauration du fichier backup via le tableau de chunk
void write_restored_file(const char *output_filename, Chunk *chunks, int chunk_count) {
    FILE *output_file = fopen(output_filename, "wb");
    if (output_file == NULL) {
        perror("Erreur d'ouverture du fichier de restauration");
        return;
    }

    // Écrire chaque chunk dans le fichier restauré
    for (int i = 0; i < chunk_count; i++) {
        fwrite(chunks[i].data, 1, CHUNK_SIZE, output_file);
    }

    fclose(output_file);
}

// Fonction pour restaurer une sauvegarde
void restore_backup(const char *backup_id, const char *restore_dir) {
    FILE *backup_file = fopen(backup_id, "rb");
    if (backup_file == NULL) {
        perror("Erreur d'ouverture du fichier de sauvegarde");
        return;
    }

    // Tableau pour stocker les chunks
    Chunk *chunks = NULL;
    int chunk_count = 0;

    // Lire les chunks du fichier de sauvegarde et les restaurer
    unsigned char buffer[CHUNK_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, backup_file)) > 0) {
        // Ajouter la logique pour restaurer chaque chunk dans le répertoire de destination
        char restored_file_path[256];
        snprintf(restored_file_path, sizeof(restored_file_path), "%s/restored_file", restore_dir);
        
        FILE *restored_file = fopen(restored_file_path, "wb");
        if (restored_file == NULL) {
            perror("Erreur d'ouverture du fichier restauré");
            continue;
        }
        
        fwrite(buffer, 1, bytes_read, restored_file);
        fclose(restored_file);
    }

    fclose(backup_file);
}
