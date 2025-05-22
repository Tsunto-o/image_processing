#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"

/*
   unsigned char header[54];
   unsigned char colorTable[1024];
   unsigned char * data;

   unsigned int width;
   unsigned int height;
   unsigned int colorDepth;
   unsigned int dataSize;
*/

t_bmp8 * bmp8_loadImage(const char * filename){
   /* permet de lire une image en niveaux de gris à partir d’un fichier BMP dont le nom (chemin) renseigné par le paramètre filename */

   FILE *fichier = fopen(filename, "rb"); //Ouverture du fichier

   //message d'erreur en cas d'échec d'ouverture du fichier ou fichier vide
   if (fichier == NULL) {
      perror("Erreur d'ouverture");
      return NULL;
   }

   t_bmp8 * img = malloc(sizeof(t_bmp8)); //allocation dynamique pour l'image

   //Lecture de l'en tête et écriture dans img->header + lecture autres champs
   fread(img->header, 54, 1, fichier);
   img->width = *(unsigned int *)&img->header[18];
   img->height = *(unsigned int *)&img->header[22];
   img->colorDepth = *(unsigned int *)&img->header[28];
   img->dataSize = ((img->width * img->colorDepth + 31) / 32) * 4 * img->height;
   fread(img->colorTable, 1024, 1, fichier); // Lis la palette

   //message d'erreur si profondeur diff de 8 bits
   if (img->colorDepth != 8){
      perror("Erreur : profondeur de couleur différente de 8bits");
      return NULL;
   }

   //lecture offset de début des données d'image
   unsigned int offset = *(unsigned int *)&img->header[10];

   //alloc dynamique pour data
   unsigned char *data = malloc(sizeof(unsigned char) * img->dataSize);

   //se positionner au début des données
   fseek(fichier, offset, SEEK_SET);
   fread(data, img->dataSize, 1, fichier);
   img->data = data;

   //fermeture fichier
   fclose(fichier);
   return img;
}



void bmp8_saveImage(const char *filename, t_bmp8 *img) {
   /*permet d’écrire une image en niveaux de gris dans un fichier BMP dont le nom (chemin) est renseigné par le paramètre filename */

   //Ouverture du fichier et vérification erreur
   FILE *fichier = fopen(filename, "wb");
   if (fichier == NULL) {
      perror("Erreur lors de l'ouverture du fichier en écriture");
      return;
   }

   //Ecriture en-tête BMP (54oc)
   fwrite(img->header, 1, 54, fichier);

   //Ecriture table couleurs (palette 8 bits = 1024 octets)
   fwrite(img->colorTable, 1, 1024, fichier);

   //Ecriture données de l'image
   size_t ecrits = fwrite(img->data, 1, img->dataSize, fichier);
   if (ecrits != img->dataSize) {
      perror("Erreur lors de l'écriture des données d'image");
   }

   fclose(fichier);
}



void bmp8_free(t_bmp8 *img) {
   /*  permet de libérer la mémoire allouée pour stocker une image de type t_bmp8. */
   if (img != NULL) {
      if (img->data != NULL) {
         free(img->data);  // Free tableau données
      }
      free(img);  // Free structure
   }
}



void bmp8_printInfo(t_bmp8 *img) {
   /* permet d’afficher les informations d’une image de type t_bmp8. Cette fonction prend en paramètre un pointeur vers une image de type t_bmp8 */
   //Vérification image valide
   if (img == NULL) {
      printf("Erreur : image NULL\n");
      return;
   }

   printf("Image Info:\n");
   printf("Width: %u\n", img->width);
   printf("Height: %u\n", img->height);
   printf("Color Depth: %u\n", img->colorDepth);
   printf("Data Size: %u\n", img->dataSize);
}



void bmp8_negative(t_bmp8 *img) {
   /* permet d’inverser les couleurs d’une image en niveaux de gris. Elle prend en paramètre un pointeur vers une image de type t_bmp8 */
   //Vérification image valide
   if (img == NULL || img->data == NULL) {
      return;
   }

   for (unsigned int i = 0; i < img->dataSize; i++) {
      img->data[i] = 255 - img->data[i];
   }
}



void bmp8_brightness(t_bmp8 *img, int value) {
   /* permet de modifier la luminosité d’une image en niveaux de gris. Elle prend en paramètre un pointeur vers une image de type t_bmp8 et un entier value (qui peut être négatif)*/
   //Vérification image valide
   if (img == NULL || img->data == NULL) {
      return;
   }

   for (unsigned int i = 0; i < img->dataSize; i++) {
      int pixel = img->data[i] + value;
      if (pixel > 255) {
         pixel = 255;
      } else if (pixel < 0) {
         pixel = 0;
      }

      img->data[i] = (unsigned char)pixel;
   }
}



void bmp8_threshold(t_bmp8 *img, int threshold) {
   /* permet de transformer une image en niveaux de gris en une image binaire. Elle prend en paramètre un pointeur vers une image de type t_bmp8 et un entier threshold */
   //Vérification image valide
   if (img == NULL || img->data == NULL) {
      return;
   }

   for (unsigned int i = 0; i < img->dataSize; i++) {
      if (img->data[i] >= threshold) {
         img->data[i] = 255;
      } else {
         img->data[i] = 0;
      }
   }
}



void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
   /* applique un filtre sur une image en niveaux de gris. Cette fonction prend en paramètre un pointeur vers une image de type t_bmp8, une matrice de flottants kernel représentant le noyau du filtre et un entier kernelSize représentant la taille du noyau */
   //Vérification image valide, kernell null
   if (img == NULL || img->data == NULL || kernel == NULL) {
      return;
   }

   int width = img->width;
   int height = img->height;
   int n = kernelSize / 2;

   // copie image pour stocker nouvelles valeurs
   unsigned char *newData = malloc(img->dataSize);
   if (newData == NULL) {
      perror("Erreur malloc dans bmp8_applyFilter");
      return;
   }

   // copie anciennes données pour pas modifier principales
   for (unsigned int i = 0; i < img->dataSize; i++) {
      newData[i] = img->data[i];
   }

   // application filtre (convolution)
   for (int y = n; y < height - n; y++) {
      for (int x = n; x < width - n; x++) {
         float sum = 0.0f;

         for (int j = -n; j <= n; j++) {
            for (int i = -n; i <= n; i++) {
               int pixelX = x + i;
               int pixelY = y + j;
               unsigned char pixel = img->data[pixelY * width + pixelX];
               sum += kernel[j + n][i + n] * pixel;
            }
         }

         if (sum < 0.0f) sum = 0.0f;
         if (sum > 255.0f) sum = 255.0f;

         newData[y * width + x] = (unsigned char)(sum);
      }
   }

   // Remplacer anciennes données par nouvelles
   for (unsigned int i = 0; i < img->dataSize; i++) {
      img->data[i] = newData[i];
   }

   free(newData); // libère mémoire temporaire
}