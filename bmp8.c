#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"


t_bmp8 * bmp8_loadImage(const char * filename){
   FILE *file = fopen(filename, "rb");
   printf(file);
}