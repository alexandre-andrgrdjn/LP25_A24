#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

void send_data(const char *server_address, int port, const void *data, size_t size) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erreur de socket");
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_address, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur de connexion");
        close(sock);
        return;
    }

    if (send(sock, data, size, 0) < 0) {
        perror("Erreur d'envoi");
    }

    close(sock);
}

void receive_data(int port, void **data, size_t *size) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erreur de socket");
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur de binding");
        close(sock);
        return;
    }

    listen(sock, 1);

    int client_sock = accept(sock, NULL, NULL);
    if (client_sock < 0) {
        perror("Erreur d'acceptation");
        close(sock);
        return;
    }

    // Allouer de la mémoire pour les données
    *data = malloc(1024);  // Allouer une taille initiale pour recevoir les données
    if (*data == NULL) {
        perror("Erreur d'allocation mémoire");
        close(client_sock);
        close(sock);
        return;
    }

    *size = recv(client_sock, *data, 1024, 0);  // Lecture de la donnée
    if (*size < 0) {
        perror("Erreur de réception");
        free(*data);  // Libérer la mémoire allouée en cas d'erreur
    }

    close(client_sock);
    close(sock);
}
