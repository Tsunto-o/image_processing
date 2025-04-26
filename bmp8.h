
typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char * data;
    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
} t_bmp8;


t_bmp8 * bmp8_loadImage(const char * filename);
/* permet de lire une image en niveaux de gris à partir d’un fichier BMP dont le nom (chemin) renseigné par le paramètre filename */

void bmp8_saveImage(const char * filename, t_bmp8 * img);
/* permet d’écrire une image en niveaux de gris dans un fichier BMP dont le nom (chemin) est renseigné par le paramètre filename */

void bmp8_free(t_bmp8 * img);
/* permet de libérer la mémoire allouée pour stocker une image de type t_bmp8. */

void bmp8_printInfo(t_bmp8 * img);
/* permet d’afficher les informations d’une image de type t_bmp8. Cette fonction prend en paramètre un pointeur vers une image de type t_bmp8 */

void bmp8_negative(t_bmp8 * img);
/* permet d’inverser les couleurs d’une image en niveaux de gris. Elle prend en paramètre un pointeur vers une image de type t_bmp8 */

void bmp8_brightness(t_bmp8 * img, int value);
/* permet de modifier la luminosité d’une image en niveaux de gris. Elle prend en paramètre un pointeur vers une image de type t_bmp8 et un entier value (qui peut être négatif)*/

void bmp8_threshold(t_bmp8 * img, int threshold);
/* permet de transformer une image en niveaux de gris en une image binaire. Elle prend en paramètre un pointeur vers une image de type t_bmp8 et un entier threshold */

void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize);
/* applique un filtre sur une image en niveaux de gris. Cette fonction prend en paramètre un pointeur vers une image de type t_bmp8, une matrice de flottants kernel représentant le noyau du filtre et un entier kernelSize représentant la taille du noyau */
