#include "backup_manager.h"
#include "deduplication.h"
#include "file_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 4096 ///< Taille des chunks (blocs) de données en octets
#define BACKUP_DIR "./backup" ///< Répertoire pour stocker les fichiers de sauvegarde
#define RESTORE_DIR "./restore" ///< Répertoire pour restaurer les fichiers

/**
 * @brief Enregistre un tableau de chunks dédupliqués dans un fichier de sauvegarde.
 *
 * @param output_filename Nom du fichier de sortie.
 * @param chunks Tableau contenant les chunks dédupliqués.
 * @param chunk_count Nombre total de chunks dans le tableau.
 */
void write_backup_file(const char *output_filename, Chunk *chunks, int chunk_count) {
    FILE *output_file = fopen(output_filename, "wb");
    if (output_file == NULL) {
        perror("Erreur d'ouverture du fichier de sauvegarde");
        return;
    }

    // Écrit chaque chunk dans le fichier de sauvegarde
    for (int i = 0; i < chunk_count; i++) {
        fwrite(chunks[i].data, 1, CHUNK_SIZE, output_file); // Écriture des données d'un chunk
    }

    fclose(output_file);
}

/**
 * @brief Sauvegarde un fichier en utilisant la déduplication.
 *
 * Cette fonction divise le fichier en chunks, applique la déduplication pour 
 * éviter de sauvegarder des données redondantes, puis écrit le résultat dans un fichier de sauvegarde.
 *
 * @param filename Nom du fichier à sauvegarder.
 */
void backup_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Erreur d'ouverture du fichier à sauvegarder");
        return;
    }

    Chunk chunks[1000]; ///< Tableau de stockage des chunks
    Md5Entry hash_table[HASH_TABLE_SIZE] = {0}; ///< Table de hachage pour détecter les doublons
    int chunk_count = 0; ///< Nombre total de chunks traités

    // Appelle la fonction de déduplication pour remplir le tableau `chunks`
    deduplicate_file(file, chunks, hash_table);
    fclose(file);

    // Sauvegarde les chunks dédupliqués dans un fichier
    write_backup_file("backup_data.sav", chunks, chunk_count);
}

/**
 * @brief Restaure un fichier à partir d'un tableau de chunks dédupliqués.
 *
 * @param output_filename Nom du fichier de sortie restauré.
 * @param chunks Tableau contenant les chunks restaurés.
 * @param chunk_count Nombre total de chunks dans le tableau.
 */
void write_restored_file(const char *output_filename, Chunk *chunks, int chunk_count) {
    FILE *output_file = fopen(output_filename, "wb");
    if (output_file == NULL) {
        perror("Erreur d'ouverture du fichier de restauration");
        return;
    }

    // Écrit chaque chunk dans le fichier restauré
    for (int i = 0; i < chunk_count; i++) {
        fwrite(chunks[i].data, 1, CHUNK_SIZE, output_file); // Écriture séquentielle des chunks
    }

    fclose(output_file);
}

/**
 * @brief Restaure une sauvegarde en lisant les chunks depuis un fichier de sauvegarde.
 *
 * Cette fonction lit les chunks sauvegardés, puis les restaure sous forme de fichiers 
 * individuels dans le répertoire spécifié.
 *
 * @param backup_id Chemin vers le fichier de sauvegarde.
 * @param restore_dir Répertoire où les fichiers restaurés seront écrits.
 */
void restore_backup(const char *backup_id, const char *restore_dir) {
    FILE *backup_file = fopen(backup_id, "rb");
    if (backup_file == NULL) {
        perror("Erreur d'ouverture du fichier de sauvegarde");
        return;
    }

    Chunk chunks[1000]; ///< Tableau temporaire pour stocker les chunks
    int chunk_count = 0; ///< Nombre de chunks lus/restaurés

    unsigned char buffer[CHUNK_SIZE]; ///< Tampon pour lire les données chunk par chunk
    size_t bytes_read;

    // Boucle de lecture des chunks depuis le fichier de sauvegarde
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, backup_file)) > 0) {
        char restored_file_path[256];
        snprintf(restored_file_path, sizeof(restored_file_path), "%s/restored_file_%d", restore_dir, chunk_count);

        FILE *restored_file = fopen(restored_file_path, "wb");
        if (restored_file == NULL) {
            perror("Erreur d'ouverture du fichier restauré");
            continue; // Ignore ce chunk si l'ouverture échoue
        }

        fwrite(buffer, 1, bytes_read, restored_file); // Écrit le chunk dans le fichier
        fclose(restored_file);
        chunk_count++;
    }

    fclose(backup_file);
}


// Point d'entrée principal pour effectuer la sauvegarde ou la restauration
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <backup|restore> <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "backup") == 0) {
        backup_file(argv[2]);
    } else if (strcmp(argv[1], "restore") == 0) {
        restore_backup(argv[2], RESTORE_DIR);
    } else {
        fprintf(stderr, "Commande non reconnue. Utilisez 'backup' ou 'restore'.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
