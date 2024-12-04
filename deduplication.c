#include "deduplication.h"

// Fonction pour calculer l'index de hachage à partir d'un MD5
unsigned int hash_md5(const unsigned char *md5) {
    unsigned int hash = 0;
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        hash = (hash << 5) + hash + md5[i]; // Hash simple (hash * 33 + md5[i])
    }
    return hash % HASH_TABLE_SIZE;
}

// Fonction pour calculer le MD5 d'un chunk de données
void compute_md5(const void *data, size_t len, unsigned char *md5_out) {
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, data, len);
    MD5_Final(md5_out, &ctx);
}

// Cherche un MD5 dans la table de hachage
int find_md5(const Md5Entry *hash_table, const unsigned char *md5) {
    unsigned int index = hash_md5(md5);
    if (memcmp(hash_table[index].md5, md5, MD5_DIGEST_LENGTH) == 0) {
        return hash_table[index].index;
    }
    return -1; // MD5 non trouvé
}

// Ajoute un MD5 dans la table de hachage
void add_md5(Md5Entry *hash_table, const unsigned char *md5, int index) {
    unsigned int hash_index = hash_md5(md5);
    memcpy(hash_table[hash_index].md5, md5, MD5_DIGEST_LENGTH);
    hash_table[hash_index].index = index;
}

// Fonction pour dédupliquer un fichier
void deduplicate_file(FILE *file, Chunk *chunks, Md5Entry *hash_table, int *chunk_count) {
    unsigned char buffer[CHUNK_SIZE];
    size_t bytes_read;
    unsigned char md5[MD5_DIGEST_LENGTH];
    *chunk_count = 0;

    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
        // Calculer le MD5 pour le chunk actuel
        compute_md5(buffer, bytes_read, md5);

        // Vérifier si ce MD5 existe déjà dans la table
        int index = find_md5(hash_table, md5);
        if (index == -1) {
            // MD5 non trouvé : ajouter le chunk dans la liste et dans la table
            add_md5(hash_table, md5, *chunk_count);
            chunks[*chunk_count].data = malloc(bytes_read);
            memcpy(chunks[*chunk_count].data, buffer, bytes_read);
            memcpy(chunks[*chunk_count].md5, md5, MD5_DIGEST_LENGTH);
            chunks[*chunk_count].size = bytes_read;
            (*chunk_count)++;
        } else {
            // MD5 trouvé : faire référence à l'entrée existante
            memcpy(chunks[*chunk_count].md5, hash_table[index].md5, MD5_DIGEST_LENGTH);
            chunks[*chunk_count].data = NULL; // Pas besoin de stocker les données
            chunks[*chunk_count].size = 0;
            (*chunk_count)++;
        }
    }
}

// Fonction pour reconstituer un fichier depuis les chunks
void undeduplicate_file(FILE *file, const Chunk *chunks, int chunk_count) {
    for (int i = 0; i < chunk_count; i++) {
        if (chunks[i].data != NULL) {
            fwrite(chunks[i].data, 1, chunks[i].size, file);
        }
    }
}
