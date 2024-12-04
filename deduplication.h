#ifndef DEDUPLICATION_H
#define DEDUPLICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

// Taille d'un chunk en octets
#define CHUNK_SIZE 4096

// Taille de la table de hachage pour éviter les collisions
#define HASH_TABLE_SIZE 1000

// Structure représentant un chunk de données
typedef struct {
    unsigned char md5[MD5_DIGEST_LENGTH]; // MD5 du chunk
    void *data; // Données brutes du chunk
    size_t size; // Taille des données du chunk
} Chunk;

// Structure pour une entrée dans la table de hachage MD5
typedef struct {
    unsigned char md5[MD5_DIGEST_LENGTH]; // MD5 du chunk
    int index; // Index du chunk correspondant
} Md5Entry;

// Fonction pour calculer l'index dans la table de hachage à partir d'un MD5
unsigned int hash_md5(const unsigned char *md5);

// Fonction pour calculer le MD5 d'un chunk de données
void compute_md5(const void *data, size_t len, unsigned char *md5_out);

// Cherche un MD5 dans la table de hachage, retourne l'index ou -1 si absent
int find_md5(const Md5Entry *hash_table, const unsigned char *md5);

// Ajoute un MD5 et son index à la table de hachage
void add_md5(Md5Entry *hash_table, const unsigned char *md5, int index);

// Découpe un fichier en chunks dédupliqués
void deduplicate_file(FILE *file, Chunk *chunks, Md5Entry *hash_table, int *chunk_count);

// Reconstitue un fichier depuis les chunks
void undeduplicate_file(FILE *file, const Chunk *chunks, int chunk_count);

#endif // DEDUPLICATION_H
