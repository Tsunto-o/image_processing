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
   //Ouverture du fichier
   FILE *fichier = fopen(filename, "rb");
   //message d'erreur en cas d'échec d'ouverture du fichier ou fichier vide
   if (fichier == NULL) {
      perror("Erreur d'ouverture");
      return NULL;
   }

   //allocation dynamique pour l'image
   t_bmp8 * img = malloc(sizeof(t_bmp8));

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

   // Lecture offset de début des données d'image
   unsigned int offset = *(unsigned int *)&img->header[10];

   //alloc dynamique pour data
   unsigned char *data = malloc(sizeof(unsigned char) * img->dataSize);

   // Se positionner au début des données
   fseek(fichier, offset, SEEK_SET);
   fread(data, img->dataSize, 1, fichier);
   img->data = data;

   //fermeture fichier
   fclose(fichier);
   return img;
}


void bmp8_saveImage(const char *filename, t_bmp8 *img) {
   //Ouverture du fichier
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