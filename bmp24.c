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
    if (fichier == NULL) {
        perror("Erreur d'ouverture");
        return NULL;
    }

    t_bmp24 * img = malloc(sizeof(t_bmp24)); //allocation dynamique pour l'image


    unsigned char * header = malloc(54*sizeof(unsigned char));
    fread(header, 54, 1, fichier);

    img->header.type= *(uint16_t *)&header[2];
    img->header.size = *(uint32_t *)&header[4];
    img->header.reserved2= *(uint16_t *)&header[6];
    img->header.reserved2 = *(uint16_t *)&header[8];
    img->header.offset = *(uint32_t *)&header[10];

    img->header_info.size = *(uint32_t *)&header[4];
    img->header_info.width = *(int32_t *)&header[18];
    img->header_info.height = *(int32_t *)&header[22];
    img->header_info.planes = *(uint16_t *)&header[26];
    img->header_info.bits = *(uint16_t *)&header[28];
    img->header_info.compression = *(uint32_t *)&header[30];
    img->header_info.imagesize = *(uint32_t *)&header[34];
    img->header_info.xresolution = *(int32_t*)&header[38];
    img->header_info.yresolution = *(int32_t *)&header[42];
    img->header_info.ncolors = *(uint32_t *)&header[46];
    img->header_info.importantcolors = *(uint32_t *)&header[50];


    printf("%x", *(unsigned int *)&header[0]);

}




