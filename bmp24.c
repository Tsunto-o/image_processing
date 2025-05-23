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


void bmp24_readPixelData(t_bmp24 *image, FILE *file) {
    if (!image || !file) return;
    for (int y = 0; y <= image->height - 1; y++) {
        for (int x = 0; x < image->width; x++) {
            uint8_t couleurs[3];
            if (fread(couleurs, 1, 3, file) != 3) {
                return;
            }
            image->data[y][x].blue  = couleurs[0];
            image->data[y][x].green = couleurs[1];
            image->data[y][x].red   = couleurs[2];


        }
    }
}

void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    if (!image || !file || !image->data) return;

    // offset
    uint32_t base_offset = image->header.offset;

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            int inverted_y = image->height - 1 - y;
            uint32_t pixel_offset = base_offset + (inverted_y * image->width + x) * 3;

            // Préparer les données en BGR
            uint8_t couleurs[3];
            couleurs[0] = image->data[y][x].blue;
            couleurs[1] = image->data[y][x].green;
            couleurs[2] = image->data[y][x].red;

            file_rawWrite(pixel_offset, couleurs, sizeof(uint8_t), 3, file);
        }
    }
}






t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (img == NULL) {
        fclose(file);
        return NULL;
    }

    file_rawRead(0, &img->header, sizeof(t_bmp_header), 1, file);
    file_rawRead(14, &img->header_info, sizeof(t_bmp_info), 1, file);

    img->width = img->header_info.width;
    img->height = img->header_info.height;
    img->colorDepth = img->header_info.bits;


    img->data = bmp24_allocateDataPixels(img->width, img->height);
    if (img->data == NULL) {
        bmp24_free(img);
        fclose(file);
        return NULL;
    }


    bmp24_readPixelData(img, file);

    fclose(file);
    return img;
}



void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    /*permet d’écrire une image en niveaux de gris dans un fichier BMP dont le nom (chemin) est renseigné par le paramètre filename */

    //Ouverture du fichier et vérification erreur
    FILE *fichier = fopen(filename, "wb");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier en écriture");
        return;
    }

    //Ecriture en-tête BMP (54oc)
    fwrite(&img->header, 1, sizeof(t_bmp_header), fichier);

    //Ecriture table couleurs (palette 8 bits = 1024 octets)
    fwrite(&img->header, 1, sizeof(t_bmp_header), fichier);

    //Ecriture données de l'image
    size_t ecrits = fwrite(img->data, 1, img->dataSize, fichier);
    if (ecrits != img->dataSize) {
        perror("Erreur lors de l'écriture des données d'image");
    }

    fclose(fichier);
}
