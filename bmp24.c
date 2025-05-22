#include <stdio.h>
#include <stdlib.h>
#include "bmp24.h"

// Constantes pour les offsets des champs de l'en-tête BMP
#define BITMAP_MAGIC 0x00 // offset 0
#define BITMAP_SIZE 0x02 // offset 2
#define BITMAP_OFFSET 0x0A // offset 10
#define BITMAP_WIDTH 0x12 // offset 18
#define BITMAP_HEIGHT 0x16 // offset 22
#define BITMAP_DEPTH 0x1C // offset 28
#define BITMAP_SIZE_RAW 0x22 // offset 34
// Constante pour le type de fichier BMP
#define BMP_TYPE 0x4D42 // 'BM' en hexadécimal
// Constantes pour les tailles des champs de l'en-tête BMP
#define HEADER_SIZE 0x0E // 14 octets
#define INFO_SIZE 0x28 // 40 octets
// Constantes pour les valeurs de profondeur de couleur
#define DEFAULT_DEPTH 0x18 // 24



t_pixel **bmp24_allocateDataPixels(int width, int height) {
    //allouer tableau de pointeurs vers ppointeurs
    t_pixel**pixels = malloc(height * sizeof(t_pixel *));
    if (pixels == NULL) {
        return NULL;
    }

    //allouer tableau pour chaque pointeur vers tableau de pointeurs
    for (int i = 0; i < height; i++) {
        pixels[i] = malloc(width * sizeof(t_pixel));
        // si erreur
        if (pixels[i] == NULL) {
            //libérer ce qui a déjà été aloué
            for (int j = 0; j < i; j++) {
                free(pixels[j]);
            }
            free(pixels);
            return NULL;
        }
    }

    return pixels;
}



void bmp24_freeDataPixels (t_pixel ** pixels, int height) {
    if (pixels == NULL) {
        return;
    }
    for (int i = 0; i < height; i++) {
        free(pixels[i]); //libérer tableau de pointeurs vers pixels
    }
    free(pixels);//libérer tableau de tableaux de pointeurs vers pixels
}



t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    // Allouer la structure t_bmp24
    t_bmp24 *image = malloc(sizeof(t_bmp24));
    if (image == NULL) {
        fprintf(stderr, "Erreur lors de l'allocation de la structure t_bmp24.\n");
        return NULL;
    }

    // Allouer la matrice de pixels
    image->data = bmp24_allocateDataPixels(width, height);
    if (image->data == NULL) {
        // Libérer l’image si l’allocation de data échoue
        free(image);
        fprintf(stderr, "Erreur lors de l'allocation de la matrice de pixels.\n");
        return NULL;
    }

    // Initialiser les champs
    image->width = width;
    image->height = height;
    image->colorDepth = colorDepth;

    return image;
}



void bmp24_free(t_bmp24 *img) {
    if (img == NULL) {
        return; // Rien à faire
    }
    if (img->data != NULL) {
        bmp24_freeDataPixels(img->data, img->height);
    }
    free(img);
}




void file_rawRead (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}

void file_rawWrite (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}






t_bmp24 * bmp24_loadImage(const char * filename) {
    /* permet de lire une image en niveaux de gris à partir d’un fichier BMP dont le nom (chemin) renseigné par le paramètre filename */

    FILE *fichier = fopen(filename, "rb"); //Ouverture du fichier
    //message d'erreur en cas d'échec d'ouverture du fichier ou fichier vide
    if (!fichier) {
        perror("Erreur d'ouverture");
        return NULL;
    }

    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (!img) {
        perror("Erreur d'allocation de l'image");
        fclose(fichier);
        return NULL;
    }

    //de 0 à 14 dans img.header
    file_rawRead(BITMAP_MAGIC, &img->header, sizeof(t_bmp_header), 1, fichier);

    //de 14 à suite dans img.header_info
    file_rawRead(14, &img->header_info, sizeof(t_bmp_info), 1, fichier);

    //vraie taille de l'image
    img->header_info.imagesize = img->header_info.width * img->header_info.height;


    //allouer matrice pixels
    img->data = bmp24_allocateDataPixels(img->width, img->height);
    if (!img->data) {
        fprintf(stderr, "Erreur d'allocation des pixels.\n");
        free(img);
        fclose(fichier);
        return NULL;
    }

    //remplir matrice
    for (int i = img->height - 1; i >= 0; i--) {
        fread(img->data[i], sizeof(t_pixel), img->width, fichier);
    }

    fclose(fichier);
    return img;
}



void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier pour écriture");
        return;
    }

    // Écriture de l'en-tête BMP (14 octets)
    file_rawWrite(0, &img->header, sizeof(t_bmp_header), 1, file);

    // Écriture de l'en-tête info BMP (40 octets)
    file_rawWrite(14, &img->header_info, sizeof(t_bmp_info), 1, file);

    // Calcul du padding pour que chaque ligne soit un multiple de 4 octets
    int rowSize = img->width * 3;
    int padding = (4 - (rowSize % 4)) % 4;
    uint8_t pad[3] = {0};

    // Positionner le curseur au début des données de pixels
    fseek(file, img->header.offset, SEEK_SET);

    // Écrire les pixels de bas en haut (format BMP 24 bits)
    for (int i = img->height - 1; i >= 0; i--) {
        fwrite(img->data[i], sizeof(t_pixel), img->width, file);
        fwrite(pad, 1, padding, file);  // Ajout du padding si nécessaire
    }

    fclose(file);
}

