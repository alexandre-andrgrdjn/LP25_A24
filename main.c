#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "file_handler.h"
#include "deduplication.h"
#include "backup_manager.h"
#include "network.h"

// Fonction pour effectuer une sauvegarde
void perform_backup(const char *source_dir, const char *backup_dir) {
    printf("Démarrage de la sauvegarde...\n");
    
    // Logique de sauvegarde
    // 1. Lister les fichiers à sauvegarder
    list_files(source_dir);

    // 2. Processus de déduplication et stockage dans le répertoire de sauvegarde

    
    printf("Sauvegarde terminée.\n");
}

// Fonction pour effectuer une restauration
void perform_restore(const char *backup_dir, const char *restore_dir) {
    printf("Démarrage de la restauration...\n");

    // Logique de restauration
    // 1. Charger les fichiers dédupliqués depuis le répertoire de sauvegarde
    // 2. Effectuer la restauration des fichiers dans le répertoire spécifié.

    printf("Restauration terminée.\n");
}

// Fonction pour afficher l'aide
void print_help() {
    printf("Usage : \n");
    printf("  --backup <source_dir> <backup_dir>  : Effectue une sauvegarde des fichiers\n");
    printf("  --restore <backup_dir> <restore_dir> : Restaure les fichiers depuis le répertoire de sauvegarde\n");
    printf("  --help                            : Affiche cette aide\n");
}

int main(int argc, char *argv[]) {
    int option;
    
    // Définir les options de la ligne de commande
    static struct option long_options[] = {
        {"backup", required_argument, NULL, 'b'},
        {"restore", required_argument, NULL, 'r'},
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0}
    };

    // Analyser les options
    while ((option = getopt_long(argc, argv, "b:r:h", long_options, NULL)) != -1) {
        switch (option) {
            case 'b': {
                // Sauvegarde
                if (optind < argc) {
                    const char *source_dir = argv[optind-1];
                    const char *backup_dir = argv[optind];
                    perform_backup(source_dir, backup_dir);
                } else {
                    printf("Erreur : répertoire de sauvegarde manquant.\n");
                    print_help();
                }
                break;
            }
            case 'r': {
                // Restauration
                if (optind < argc) {
                    const char *backup_dir = argv[optind-1];
                    const char *restore_dir = argv[optind];
                    perform_restore(backup_dir, restore_dir);
                } else {
                    printf("Erreur : répertoire de restauration manquant.\n");
                    print_help();
                }
                break;
            }
            case 'h':
                // Afficher l'aide
                print_help();
                break;
            default:
                // Si option inconnue
                print_help();
                break;
        }
    }

    return EXIT_SUCCESS;
}
