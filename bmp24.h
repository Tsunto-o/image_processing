#include <stdint.h>

typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;

typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t imagesize;
    int32_t xresolution;
    int32_t yresolution;
    uint32_t ncolors;
    uint32_t importantcolors;
} t_bmp_info;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;


typedef struct {
    t_bmp_header header;
    t_bmp_info header_info;
    int width;
    int height;
    int colorDepth;
    t_pixel **data;
} t_bmp24;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


t_pixel ** bmp24_allocateDataPixels (int width, int height);
/* Alloue dynamiquement de la mémoire pour une matrice de t_pixel de taille witdh × height, et renvoyer l’adresse allouée dans le tas */

void bmp24_freeDataPixels (t_pixel ** pixels, int height);
/* Libère toute la mémoire allouée pour la matrice de t_pixel pixels. */


t_bmp24 * bmp24_allocate (int width, int height, int colorDepth);
/* Alloue dynamiquement de la mémoire pour une image t_bmp24 */

void bmp24_free (t_bmp24 * img);
/* Libère toute la mémoire allouée pour l’image img reçue en paramètre. */



void file_rawRead (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file);
/* lit à une position donnée dans un fichier donné */

void file_rawWrite (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file);
/* écrit à une position donnée dans un fichier donné */


void bmp24_readPixelValue (t_bmp24 * image, int x, int y, FILE * file);
/* lit la valeur d’un pixel à une position donnée (x, y) dans le fichier file */

void bmp24_writePixelValue (t_bmp24 * image, int x, int y, FILE * file);
/* écrit la valeur d’un pixel à une position donnée (x, y) dans le fichier file */


void bmp24_readPixelData (t_bmp24 * image, FILE * file);
/* lit toutes les données de l’image dans le fichier file */

void bmp24_writePixelData (t_bmp24 * image, FILE * file);
/* écrit toutes les données de l’image dans le fichier file */



t_bmp24 * bmp24_loadImage (const char * filename);
/* lit un fichier image au format BMP 24 bits et le charger dans une structure de type t_bmp24. */

void bmp24_saveImage (t_bmp24 * img, const char * filename);
/* sauvegarde une image de type t_bmp24 dans un fichier au format BMP 24 bits */


void bmp24_negative(t_bmp24 *img);
/* Inverser les couleurs de l’image : pour chaque pixel, soustraire la valeur de chaque canal de couleur à 255. */

void bmp24_grayscale (t_bmp24 * img);
/* Convertir l’image en niveaux de gris : pour chaque pixel, calculer la valeur moyenne des 3 canaux de couleur et affecter cette valeur à chaque canal */

void bmp24_brightness (t_bmp24 * img, int value);
/* Ajuster la luminosité de l’image : pour chaque pixel, ajouter une valeur value à chaque canal de couleur. */



void bmp24_computeHistograms(t_bmp24 *img, unsigned int *histR, unsigned int *histG, unsigned int *histB);
/* renvoie un tableau d’entiers contenant l’histogramme */

unsigned int * bmp24_computeCDF(unsigned int * hist);
/* renvoie un tableau d’entiers contenant l’histogramme cumulé puis normalisé hist_eq. */

void bmp24_equalize(t_bmp24 * img);
/* effectue l’égalisation d’histogramme sur une image couleur 24-bits */

