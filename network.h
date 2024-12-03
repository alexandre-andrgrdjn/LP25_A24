#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>  // Ajouter cette ligne pour résoudre l'erreur sur size_t

void send_data(const char *server_address, int port, const void *data, size_t size);
void receive_data(int port, void **data, size_t *size);

#endif // NETWORK_H
