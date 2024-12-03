#ifndef DEDUPLICATION_H
#define DEDUPLICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <dirent.h>

// Taille d'un chunk (4096 octets)
#define CHUNK_SIZE 4096

// Taille de la table de hachage pour les chunks déjà calculés
#define HASH_TABLE_SIZE 1000

// Structure représentant un chunk de données
typedef struct {
    unsigned char md5[MD5_DIGEST_LENGTH]; // MD5 du chunk
    void *data; // Pointeur vers les données du chunk
} Chunk;

// Structure représentant une entrée dans la table de hachage pour les MD5
typedef struct {
    unsigned char md5[MD5_DIGEST_LENGTH]; // MD5 de l'entrée
    int index; // Index du chunk associé dans le tableau
} Md5Entry;

// Fonction de hachage MD5 pour indexer un MD5 dans la table de hachage
unsigned int hash_md5(unsigned char *md5);

// Fonction pour calculer le MD5 d'un chunk de données
void compute_md5(void *data, size_t len, unsigned char *md5_out);

// Fonction pour chercher un MD5 dans la table de hachage
// Retourne l'index du chunk si trouvé, -1 sinon
int find_md5(Md5Entry *hash_table, unsigned char *md5);

// Fonction pour ajouter une entrée MD5 dans la table de hachage
// avec l'index associé du chunk
void add_md5(Md5Entry *hash_table, unsigned char *md5, int index);

// Fonction pour dédupliquer un fichier en découpant les données en chunks
// et en comparant les MD5 pour éviter les doublons
void deduplicate_file(FILE *file, Chunk *chunks, Md5Entry *hash_table);

// Fonction pour "undédupliquer" un fichier, en remplaçant les références
// de chunks par les données correspondantes dans le tableau de chunks
void undeduplicate_file(FILE *file, Chunk **chunks, int *chunk_count);

#endif // DEDUPLICATION_H
