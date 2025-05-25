#include <stdio.h>
#include <stdlib.h>
#include "bmp24.h"

#include <math.h>

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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void file_rawRead (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}

void file_rawWrite (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}


void bmp24_readPixelValue(t_bmp24 * img,  int x, int y, FILE * file){
    if (!(img->height%4 == 0) || !(img->width%4 == 0)) {
        printf("Error : invalid image size (%d,%d)\n", img->height, img->width);
        return;
    }
    //lire l'offset du fichier BMP
    uint32_t offset  = img->header.offset;
    // taille d'une ligne en octets (3 octets par pixel)
    uint32_t rowSize = img->width * 3;
    // inversion verticale (BMP stocke du bas vers le haut)
    uint32_t fileY = img->height - 1 - y;
    // position dans le fichier
    uint32_t pos = offset + fileY * rowSize + x * 3;

    unsigned char bgr[3];
    file_rawRead(pos, bgr, 1, 3, file);

    // stockage en RGB
    img->data[y][x].red   = bgr[2];
    img->data[y][x].green = bgr[1];
    img->data[y][x].blue  = bgr[0];;

}

void bmp24_readPixelData(t_bmp24 *img, FILE *file) {
    for (int y = 0; y < img->height; y++)
        for (int x = 0; x < img->width; x++)
            bmp24_readPixelValue(img, x, y, file);
}
//@brief Écrit la valeur du pixel (x,y) depuis image->data[y][x] dans le fichier BMP.

void bmp24_writePixelValue(t_bmp24 *img, int x, int y, FILE *file) {
    uint32_t offset  = img->header.offset;
    uint32_t rowsize = img->width*3;
    uint32_t fileY = (uint32_t)img->height - 1 - (uint32_t) y;
    uint32_t position = offset + fileY * rowsize + (uint32_t)x * 3;

    // Préparer les 3 octets en ordre B, G, R
    unsigned char bgr[3] = { img->data[y][x].blue, img->data[y][x].green, img->data[y][x].red };
    // Écrire ces 3 octets
    file_rawWrite(position, bgr, 1, 3, file); }


void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            bmp24_writePixelValue(image, x, y, file);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *fichier = fopen(filename, "rb");
    if (fichier == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    t_bmp24 *img = malloc(sizeof(t_bmp24));
    if (img == NULL) {
        fclose(fichier);
        return NULL;
    }

    file_rawRead(0,  &img->header.type,      sizeof(uint16_t), 1, fichier); // type
    file_rawRead(2,  &img->header.size,      sizeof(uint32_t), 1, fichier); // size
    file_rawRead(6,  &img->header.reserved1, sizeof(uint16_t), 1, fichier); // reserved1
    file_rawRead(8,  &img->header.reserved2, sizeof(uint16_t), 1, fichier); // reserved2
    file_rawRead(10, &img->header.offset,    sizeof(uint32_t), 1, fichier); // offset

    file_rawRead(14, &img->header_info, sizeof(t_bmp_info), 1, fichier);

    img->width = img->header_info.width;
    img->height = img->header_info.height;
    img->colorDepth = img->header_info.bits;


    img->data = bmp24_allocateDataPixels(img->width, img->height);
    if (img->data == NULL) {
        printf("Erreur : données de l'image erronnée\n");
        bmp24_free(img);
        fclose(fichier);
        return NULL;
    }

    bmp24_readPixelData(img, fichier);

    printf("Image chargée avec succès !\n");

    fclose(fichier);
    return img;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    //permet d’écrire une image en niveaux de gris dans un fichier BMP dont le nom (chemin) est renseigné par le paramètre filename

    //Ouverture du fichier et vérification erreur
    FILE *fichier = fopen(filename, "wb");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier en écriture");
        return;
    }


    file_rawWrite(0,  &img->header.type,      sizeof(uint16_t), 1, fichier); // type
    file_rawWrite(2,  &img->header.size,      sizeof(uint32_t), 1, fichier); // size
    file_rawWrite(6,  &img->header.reserved1, sizeof(uint16_t), 1, fichier); // reserved1
    file_rawWrite(8,  &img->header.reserved2, sizeof(uint16_t), 1, fichier); // reserved2
    file_rawWrite(10, &img->header.offset,    sizeof(uint32_t), 1, fichier); // offset

    file_rawWrite(14, &img->header_info, sizeof(t_bmp_info), 1, fichier);

    bmp24_writePixelData(img, fichier);

    printf("Image sauvegardée avec succès !\n");

    fclose(fichier);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void bmp24_applyFilter(t_bmp24 *image, float **kernel, int kernelSize) {
    if (!image || !kernel || kernelSize <= 0) return;
    int width = image->width, height = image->height;
    int offset = kernelSize / 2;

    // Allocation temporaire et copie des pixels originaux
    t_pixel **temp = bmp24_allocateDataPixels(width,height);
    if (!temp) return;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            temp[y][x] = image->data[y][x];
        }
    }
    //printf("Fin mapping pixels\n");
    // Application du filtre convolution sur chaque canal
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double sumR = 0.0, sumG = 0.0, sumB = 0.0;
            for (int ky = 0; ky < kernelSize; ky++) {
                for (int kx = 0; kx < kernelSize; kx++) {
                    int xx = x + kx - offset;
                    int yy = y + ky - offset;
                    if (xx >= 0 && xx < width && yy >= 0 && yy < height) {
                        t_pixel *p = &temp[yy][xx];
                        float kval = kernel[ky][kx];
                        sumR += p->red * kval;
                        sumG += p->green * kval;
                        sumB += p->blue * kval;
                    }
                }
            }

            // Clamp et assignation dans l'image originale
            int vR = (int)sumR; if (vR < 0) vR = 0; if (vR > 255) vR = 255;
            int vG = (int)sumG; if (vG < 0) vG = 0; if (vG > 255) vG = 255;
            int vB = (int)sumB; if (vB < 0) vB = 0; if (vB > 255) vB = 255;
            image->data[y][x].red = (unsigned char)vR;
            image->data[y][x].green = (unsigned char)vG;
            image->data[y][x].blue = (unsigned char)vB;
        }
    }
    printf("Filtre applique !\n");
    bmp24_freeDataPixels(temp,height);
}

void bmp24_negative(t_bmp24 *img) {
    int width = img->width;
    int height = img->height;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            img->data[y][x].red = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue = 255 - img->data[y][x].blue;
        }
    }
}

void bmp24_grayscale(t_bmp24 * img) {
    int width = img->width;
    int height = img->height;
    int moyenne;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            moyenne = (img->data[y][x].red + img->data[y][x].blue + img->data[y][x].green)/3;
            img->data[y][x].red = moyenne;
            img->data[y][x].blue = moyenne;
            img->data[y][x].green = moyenne;
        }
    }
}

void bmp24_brightness(t_bmp24 * img, int value) {
    int width = img->width;
    int height = img->height;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // On s'assure que la valeur ne dépasse pas 255
            if (img->data[y][x].red+value<=255)
                img->data[y][x].red+=value;
            else
                img->data[y][x].red = 255;

            if (img->data[y][x].green+value<=255)
                img->data[y][x].green+=value;
            else
                img->data[y][x].green = 255;

            if (img->data[y][x].blue+value<=255)
                img->data[y][x].blue+=value;
            else
                img->data[y][x].blue = 255;

        }
    }
}

