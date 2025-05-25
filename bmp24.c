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
    /* Allouer dynamiquement de la mémoire pour une matrice de t_pixel de taille witdh × height, et renvoyer l’adresse allouée dans le tas */
    //allouer tableau de pointeurs vers pointeurs
    t_pixel**pixels = malloc(height * sizeof(t_pixel *));
    if (pixels == NULL) {
        return NULL;
    }

    //allouer tableau pour chaque pointeur vers tableau de pointeurs
    for (int i = 0; i < height; i++) {
        pixels[i] = malloc(width * sizeof(t_pixel));
        // si erreur
        if (pixels[i] == NULL) {
            //libérer ce qui a déjà été alloué
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
    /* Libérer toute la mémoire allouée pour la matrice de t_pixel pixels. */
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
    /* Allouer dynamiquement de la mémoire pour une image t_bmp24 */
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
    /* Libérer toute la mémoire allouée pour l’image img reçue en paramètre. */

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
    /* lire la valeur d’un pixel à une position donnée (x, y) dans le fichier file */
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


void bmp24_writePixelValue(t_bmp24 *img, int x, int y, FILE *file) {
    /* écrit la valeur d’un pixel à une position donnée (x, y) dans le fichier file */
    uint32_t offset  = img->header.offset;
    uint32_t rowsize = img->width*3;
    uint32_t fileY = (uint32_t)img->height - 1 - (uint32_t) y;
    uint32_t position = offset + fileY * rowsize + (uint32_t)x * 3;

    // Préparer les 3 octets en ordre B, G, R
    unsigned char bgr[3] = { img->data[y][x].blue, img->data[y][x].green, img->data[y][x].red };
    // Écrire ces 3 octets
    file_rawWrite(position, bgr, 1, 3, file); }




void bmp24_readPixelData(t_bmp24 *img, FILE *file) {
    /* lit toutes les données de l’image dans le fichier file */
    for (int y = 0; y < img->height; y++)
        for (int x = 0; x < img->width; x++)
            bmp24_readPixelValue(img, x, y, file);
}
//@brief Écrit la valeur du pixel (x,y) depuis image->data[y][x] dans le fichier BMP.


void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    /* écrit toutes les données de l’image dans le fichier file */
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            bmp24_writePixelValue(image, x, y, file);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


t_bmp24 *bmp24_loadImage(const char *filename) {
    /* lit un fichier image au format BMP 24 bits et le charger dans une structure de type t_bmp24. */
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
    /* sauvegarde une image de type t_bmp24 dans un fichier au format BMP 24 bits */

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


void bmp24_negative(t_bmp24 *img) {
    /* Inverser les couleurs de l’image : pour chaque pixel, soustraire la valeur de chaque canal de couleur à 255. */
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
    /* Convertir l’image en niveaux de gris : pour chaque pixel, calculer la valeur moyenne des 3 canaux de couleur et affecter cette valeur à chaque canal */
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
    /* Ajuster la luminosité de l’image : pour chaque pixel, ajouter une valeur value à chaque canal de couleur. */
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize) {
    int offset = kernelSize / 2;
    float sumR = 0, sumG = 0, sumB = 0;

    for (int ky = -offset; ky <= offset; ky++) {
        for (int kx = -offset; kx <= offset; kx++) {
            int px = x + kx;
            int py = y + ky;

            // Vérifier que les indices sont dans les bornes de l'image
            if (px >= 0 && px < img->width && py >= 0 && py < img->height) {
                t_pixel p = img->data[py][px];
                float coeff = kernel[ky + offset][kx + offset];
                sumR += p.red * coeff;
                sumG += p.green * coeff;
                sumB += p.blue * coeff;
            }
        }
    }

    // Clamp les valeurs dans [0, 255]
    t_pixel result;
    result.red   = (uint8_t)(sumR > 255 ? 255 : (sumR < 0 ? 0 : sumR));
    result.green = (uint8_t)(sumG > 255 ? 255 : (sumG < 0 ? 0 : sumG));
    result.blue  = (uint8_t)(sumB > 255 ? 255 : (sumB < 0 ? 0 : sumB));
    return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
    //application filtre convolution sur chaque canal
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double sumRed = 0.0, sumGreen = 0.0, sumBlue = 0.0;
            for (int ky = 0; ky < kernelSize; ky++) {
                for (int kx = 0; kx < kernelSize; kx++) {
                    int xx = x + kx - offset;
                    int yy = y + ky - offset;
                    if (xx >= 0 && xx < width && yy >= 0 && yy < height) {
                        t_pixel *p = &temp[yy][xx];
                        float kval = kernel[ky][kx];
                        sumRed += p->red * kval;
                        sumGreen += p->green * kval;
                        sumBlue += p->blue * kval;
                    }
                }
            }

            // Clamp et assignation dans l'image originale
            int vRed = (int)sumRed;
            if (vRed < 0) vRed = 0;
            if (vRed > 255) vRed = 255;

            int vGreen = (int)sumGreen;
            if (vGreen < 0) vGreen = 0;
            if (vGreen > 255) vGreen = 255;

            int vBlue = (int)sumBlue;
            if (vBlue < 0) vBlue = 0;
            if (vBlue > 255) vBlue = 255;

            image->data[y][x].red = (unsigned char)vRed;
            image->data[y][x].green = (unsigned char)vGreen;
            image->data[y][x].blue = (unsigned char)vBlue;
        }
    }
    printf("Filtre applique !\n");
    bmp24_freeDataPixels(temp,height);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void bmp24_computeHistograms(t_bmp24 *img, unsigned int *histR, unsigned int *histG, unsigned int *histB) {
    /* renvoie un tableau d’entiers contenant l’histogramme cumulé puis normalisé hist_eq. */
    if (img == NULL || histR == NULL || histG == NULL || histB == NULL) {
        printf("Erreur : paramètres invalides pour le calcul d'histogrammes 24 bits.\n");
        return;
    }

    //initialiser histogrammes à zéro
    for (int i = 0; i < 256; i++) {
        histR[i] = 0;
        histG[i] = 0;
        histB[i] = 0;
    }

    //parcourir chaque pixel + incrémenter histogrammes
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            histR[img->data[y][x].red]++;
            histG[img->data[y][x].green]++;
            histB[img->data[y][x].blue]++;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


unsigned int * bmp24_computeCDF(unsigned int * hist) {
    /* renvoie un tableau d’entiers contenant l’histogramme */
    if (hist == NULL) {
        printf("Erreur : histogramme erroné.\n");
        return NULL;
    }

    unsigned int *cdf = calloc(256, sizeof(unsigned int)); //initialiser avec 0
    if (cdf == NULL) {
        printf("Erreur : échec d'allocation mémoire pour la CDF.\n");
        return NULL;
    }

    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + hist[i];
    }
    return cdf;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void bmp24_equalize(t_bmp24 * img) {
    /* effectue l’égalisation d’histogramme sur une image couleur 24-bits */
    if (img == NULL) {
        printf("Erreur : image erronée.\n");
        return;
    }

    // Allouer et calculer les histogrammes
    unsigned int *histR = calloc(256, sizeof(unsigned int));
    unsigned int *histG = calloc(256, sizeof(unsigned int));
    unsigned int *histB = calloc(256, sizeof(unsigned int));
    if (histR == NULL || histG == NULL || histB == NULL) {
        printf("Erreur : échec d'allocation mémoire.\n");
        free(histR); free(histG); free(histB);
        return;
    }

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            histR[img->data[y][x].red]++;
            histG[img->data[y][x].green]++;
            histB[img->data[y][x].blue]++;
        }
    }

    // Calculer les CDF
    unsigned int *cdfR = calloc(256, sizeof(unsigned int));
    unsigned int *cdfG = calloc(256, sizeof(unsigned int));
    unsigned int *cdfB = calloc(256, sizeof(unsigned int));
    if (cdfR == NULL || cdfG == NULL || cdfB == NULL) {
        printf("Erreur : échec d'allocation.\n");
        free(histR); free(histG); free(histB);
        free(cdfR); free(cdfG); free(cdfB);
        return;
    }

    cdfR[0] = histR[0];
    cdfG[0] = histG[0];
    cdfB[0] = histB[0];
    for (int i = 1; i < 256; i++) {
        cdfR[i] = cdfR[i - 1] + histR[i];
        cdfG[i] = cdfG[i - 1] + histG[i];
        cdfB[i] = cdfB[i - 1] + histB[i];
    }

    // Trouver CDF min pour chaque canal
    unsigned int cdf_minR = 0, cdf_minG = 0, cdf_minB = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf_minR == 0 && cdfR[i] != 0) cdf_minR = cdfR[i];
        if (cdf_minG == 0 && cdfG[i] != 0) cdf_minG = cdfG[i];
        if (cdf_minB == 0 && cdfB[i] != 0) cdf_minB = cdfB[i];
    }

    unsigned int totalPixels = img->width * img->height;

    // Calculer les LUT (tableaux de correspondance égalisée)
    unsigned char lutR[256], lutG[256], lutB[256];
    for (int i = 0; i < 256; i++) {
        lutR[i] = (unsigned char)(((float)(cdfR[i] - cdf_minR) / (totalPixels - cdf_minR)) * 255.0 + 0.5);
        lutG[i] = (unsigned char)(((float)(cdfG[i] - cdf_minG) / (totalPixels - cdf_minG)) * 255.0 + 0.5);
        lutB[i] = (unsigned char)(((float)(cdfB[i] - cdf_minB) / (totalPixels - cdf_minB)) * 255.0 + 0.5);
    }

    // Appliquer la transformation
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            img->data[y][x].red = lutR[img->data[y][x].red];
            img->data[y][x].green = lutG[img->data[y][x].green];
            img->data[y][x].blue = lutB[img->data[y][x].blue];
        }
    }

    // Libérer la mémoire
    free(histR); free(histG); free(histB);
    free(cdfR); free(cdfG); free(cdfB);
}