#include <stdio.h>
#include <string.h>
#include "../src/deduplication.h"

int main() {
    // Exemple de données
    const char *data = "Hello, world!";
    unsigned char md5_result[MD5_DIGEST_LENGTH];

    // Appel de compute_md5
    compute_md5((unsigned char *)data, strlen(data), md5_result);

    // Affichage du résultat MD5
    printf("MD5: ");
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        printf("%02x", md5_result[i]);
    }
    printf("\n");

    return 0;
}
