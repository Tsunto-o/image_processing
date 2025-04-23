#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bmp8.c"

int main() {
    FILE *fichier = fopen("test.txt", "r");
    if (fichier == NULL) {
        perror("Erreur d'ouverture");
        return 1;
    }

    char c;
    while ((c = fgetc(fichier)) != EOF) {
        putchar(c);
    }

    fclose(fichier);
    return 0;
}