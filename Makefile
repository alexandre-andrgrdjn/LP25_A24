# Variables de compilation
CC = gcc
CFLAGS = -Wall -Wextra -Wno-deprecated-declarations
LDFLAGS = -lssl -lcrypto  # Lier OpenSSL pour MD5

# Fichiers sources et objets
SRC = main.c file_handler.c deduplication.c backup_manager.c network.c
OBJ = $(SRC:.c=.o)

# Nom de l'exécutable
TARGET = lp25_borgbackup

# Règles
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Compilation des fichiers sources en fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJ) $(TARGET)

# Règle pour nettoyer tous les fichiers générés
fclean: clean
	rm -f $(TARGET)

# Règle pour recompiler complètement
re: fclean all
