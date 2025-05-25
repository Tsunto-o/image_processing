#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bmp8.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


t_bmp8 * bmp8_loadImage(const char * filename){
   /* permet de lire une image en niveaux de gris a partir d’un fichier BMP dont le nom (chemin) renseigne par le parametre filename */

   FILE *fichier = fopen(filename, "rb"); //Ouverture du fichier

   //message d'erreur en cas d'echec d'ouverture du fichier ou fichier vide
   if (fichier == NULL) {
      perror("Erreur d'ouverture");
      return NULL;
   }

   t_bmp8 * img = malloc(sizeof(t_bmp8)); //allocation dynamique pour l'image

   //Lecture de l'en tete et ecriture dans img->header + lecture autres champs
   fread(img->header, 54, 1, fichier);
   img->width = *(unsigned int *)&img->header[18];
   img->height = *(unsigned int *)&img->header[22];
   img->colorDepth = *(unsigned int *)&img->header[28];
   img->dataSize = img->width * img->height;
   fread(img->colorTable, 1024, 1, fichier); // Lis la palette

   //message d'erreur si profondeur diff de 8 bits
   if (img->colorDepth != 8){
      perror("Erreur : profondeur de couleur differente de 8bits");
      return NULL;
   }

   //lecture offset de debut des donnees d'image
   unsigned int offset = *(unsigned int *)&img->header[10];

   //alloc dynamique pour data
   unsigned char *data = malloc(sizeof(unsigned char) * img->dataSize);

   //se positionner au debut des donnees
   fseek(fichier, offset, SEEK_SET);
   fread(data, img->dataSize, 1, fichier);
   img->data = data;

   //fermeture fichier
   fclose(fichier);
   return img;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void bmp8_saveImage(const char *filename, t_bmp8 *img) {
   /*permet d’ecrire une image en niveaux de gris dans un fichier BMP dont le nom (chemin) est renseigne par le parametre filename */

   //Ouverture du fichier et verification erreur
   FILE *fichier = fopen(filename, "wb");
   if (fichier == NULL) {
      perror("Erreur lors de l'ouverture du fichier en ecriture");
      return;
   }

   //Ecriture en-tete BMP (54oc)
   fwrite(img->header, 1, 54, fichier);

   //Ecriture table couleurs (palette 8 bits = 1024 octets)
   fwrite(img->colorTable, 1, 1024, fichier);

   //Ecriture donnees de l'image
   size_t ecrits = fwrite(img->data, 1, img->dataSize, fichier);
   if (ecrits != img->dataSize) {
      perror("Erreur lors de l'ecriture des donnees d'image");
   }

   fclose(fichier);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void bmp8_free(t_bmp8 *img) {
   /*  permet de liberer la memoire allouee pour stocker une image de type t_bmp8. */
   if (img != NULL) {
      if (img->data != NULL) {
         free(img->data);  // Free tableau donnees
      }
      free(img);  // Free structure
   }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void bmp8_printInfo(t_bmp8 *img) {
   /* permet d’afficher les informations d’une image de type t_bmp8. Cette fonction prend en parametre un pointeur vers une image de type t_bmp8 */
   //Verification image valide
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void bmp8_negative(t_bmp8 *img) {
   /* permet d’inverser les couleurs d’une image en niveaux de gris. Elle prend en parametre un pointeur vers une image de type t_bmp8 */
   //Verification image valide
   if (img == NULL || img->data == NULL) {
      return;
   }

   for (unsigned int i = 0; i < img->dataSize; i++) {
      img->data[i] = 255 - img->data[i];
   }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void bmp8_brightness(t_bmp8 *img, int value) {
   /* permet de modifier la luminosite d’une image en niveaux de gris. Elle prend en parametre un pointeur vers une image de type t_bmp8 et un entier value (qui peut etre negatif)*/
   //Verification image valide
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void bmp8_threshold(t_bmp8 *img, int threshold) {
   /* permet de transformer une image en niveaux de gris en une image binaire. Elle prend en parametre un pointeur vers une image de type t_bmp8 et un entier threshold */
   //Verification image valide
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
   /* applique un filtre sur une image en niveaux de gris. Cette fonction prend en parametre un pointeur vers une image de type t_bmp8, une matrice de flottants kernel representant le noyau du filtre et un entier kernelSize representant la taille du noyau */
   //Verification image valide, kernell null
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

   // copie anciennes donnees pour pas modifier principales
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

   // Remplacer anciennes donnees par nouvelles
   for (unsigned int i = 0; i < img->dataSize; i++) {
      img->data[i] = newData[i];
   }

   free(newData); // libere memoire temporaire
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


unsigned int * bmp8_computeHistogram(t_bmp8 * img) {
   /*calcule l’histogramme d’une image 8-bits en niveau de gris*/
   //verification validite pointeur
   if (img == NULL || img->data == NULL) {
      return NULL;
   }

   //allocation tableau 256 entiers pour niveaux de gris
   unsigned int *histogram = (unsigned int *)calloc(256, sizeof(unsigned int));
   if (histogram == NULL) {
      return NULL; // echec de l'allocation memoire
   }

   // Parcourir pixels de l'image
   for (unsigned int i = 0; i < img->dataSize; i++) {
      unsigned char gray = img->data[i]; // Chaque pixel est un niveau de gris entre 0 et 255
      histogram[gray]++;
   }
   return histogram;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


unsigned int * bmp8_computeCDF(unsigned int * hist) {
   /* prend en entree un tableau d’entiers hist representant l’histogramme d’une image t_bmp8 et renvoie un tableau d’entiers de taille 256 contenant l’histogramme cumule puis normalise hist_eq. */
   unsigned int * cdf = malloc(256 * sizeof(unsigned int));
   if (!cdf) return NULL;  // Verification allocation

   cdf[0] = hist[0];
   for (int i = 1; i < 256; i++) {
      cdf[i] = cdf[i - 1] + hist[i];
   }

   return cdf;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void bmp8_equalize(t_bmp8 * img) {
   /* applique l’egalisation d’histogramme a une image 8-bits en niveau de gris */
   unsigned int * hist = bmp8_computeHistogram(img);
   if (!hist) return;  // securite

   unsigned int * cdf = bmp8_computeCDF(hist);
   free(hist);  // liberer l'histogramme car plus utile

   if (!cdf) return;  // securite

   // Trouver cdfmin (premier cdf non nul)
   unsigned int cdfmin = 0;
   for (int i = 0; i < 256; i++) {
      if (cdf[i] != 0) {
         cdfmin = cdf[i];
         break;
      }
   }

   // Nombre total de pixels
   unsigned int totalPixels = img->dataSize;

   // Egalisation classique : remapping des pixels
   for (unsigned int i = 0; i < totalPixels; i++) {
      unsigned int pixel = img->data[i];
      // Formule classique d'egalisation :
      // newPixel = round( (cdf[pixel] - cdfmin) / (totalPixels - cdfmin) * 255 )
      if (totalPixels == cdfmin) {
         // eviter division par zero, on laisse inchange
         img->data[i] = pixel;
      } else {
         unsigned int val = (unsigned int)( ( (double)(cdf[pixel] - cdfmin) / (totalPixels - cdfmin) ) * 255.0 + 0.5 );
         if (val > 255) val = 255;
         img->data[i] = (unsigned char) val;
      }
   }

   free(cdf);  // liberation du CDF
}