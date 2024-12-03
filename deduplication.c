#include "deduplication.h"
#include "file_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <dirent.h>



// Fonction de hachage MD5 pour l'indexation dans la table de hachage
unsigned int hash_md5(unsigned char *md5) {
    unsigned int hash = 0;
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        hash = (hash << 5) + hash + md5[i];
    }
    return hash % HASH_TABLE_SIZE;
}

void compute_md5(void *data, size_t len, unsigned char *md5_out) {
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, data, len);
    MD5_Final(md5_out, &ctx);
}

int find_md5(Md5Entry *hash_table, unsigned char *md5) {
    unsigned int index = hash_md5(md5);
    if (memcmp(hash_table[index].md5, md5, MD5_DIGEST_LENGTH) == 0) {
        return hash_table[index].index;
    }
    return -1;  // MD5 non trouvé
}

// Ajouter un MD5 dans la table de hachage
void add_md5(Md5Entry *hash_table, unsigned char *md5, int index) {
    unsigned int hash_index = hash_md5(md5);
    memcpy(hash_table[hash_index].md5, md5, MD5_DIGEST_LENGTH);
    hash_table[hash_index].index = index;
}
// Fonction pour convertir un fichier non dédupliqué en tableau de chunks
void deduplicate_file(FILE *file, Chunk *chunks, Md5Entry *hash_table) {
    char buffer[CHUNK_SIZE];
    size_t bytes_read;
    unsigned char md5[MD5_DIGEST_LENGTH];
    int chunk_count = 0;

    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
        // Calculer le MD5 du chunk
        compute_md5(buffer, bytes_read, md5);

        // Vérifier si ce MD5 existe déjà dans la table de hachage
        int index = find_md5(hash_table, md5);
        if (index == -1) {
            // Si ce MD5 n'existe pas, ajouter à la table de hachage et enregistrer le chunk
            add_md5(hash_table, md5, chunk_count);
            memcpy(chunks[chunk_count].md5, md5, MD5_DIGEST_LENGTH);
            memcpy(chunks[chunk_count].data, buffer, bytes_read);
            chunk_count++;
        } else {
            // Si le MD5 existe, simplement lier ce chunk à l'index dans la table de hachage
            memcpy(chunks[chunk_count].md5, hash_table[index].md5, MD5_DIGEST_LENGTH);
            chunk_count++;
        }
    }
}



// Fonction permettant de charger un fichier dédupliqué en table de chunks
void undeduplicate_file(FILE *file, Chunk **chunks, int *chunk_count) {
    unsigned char buffer[CHUNK_SIZE];
    size_t bytes_read;
    int current_chunk = 0;

    // Réinitialiser le nombre de chunks
    *chunk_count = 0;

    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
        unsigned char md5[MD5_DIGEST_LENGTH];
        compute_md5(buffer, bytes_read, md5);

        // Charger le chunk à partir des données
        (*chunks)[current_chunk].data = malloc(bytes_read);
        memcpy((*chunks)[current_chunk].data, buffer, bytes_read);
        memcpy((*chunks)[current_chunk].md5, md5, MD5_DIGEST_LENGTH);

        current_chunk++;
        (*chunk_count)++;
    }
}
