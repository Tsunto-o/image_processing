#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bmp24.c"
#include "bmp8.c"
#include "menu.c"

int main() {
    menu();

    //t_bmp24 * test24 = bmp24_loadImage("lena_color.bmp");
    //bmp24_negative(test24);
    //bmp24_saveImage(test24, "test2.bmp");

    /*
    t_bmp8 * test8 = bmp8_loadImage("barbara_gray.bmp");
    printf("width = %u, height = %u, dataSize = %u\n", test8->width, test8->height, test8->dataSize);


    unsigned int *hist = bmp8_computeHistogram(test8);
    if (hist != NULL) {
        for (int i = 0; i < 256; i++) {
            printf("Gris %d: %u pixels\n", i, hist[i]);
        }
        free(hist);
    }


    unsigned int *hist1 = bmp8_computeHistogram(test8);
    if (hist != NULL) {
        unsigned int *cdf = bmp8_computeCDF(hist);
        if (cdf != NULL) {
            for (int i = 0; i < 256; i++) {
                printf("CDF[%d] = %u\n", i, cdf[i]);
            }
            free(cdf);
        }
        free(hist1);
    }

    bmp8_equalize(test8);
    bmp8_saveImage("test.bmp", test8);


    t_bmp24 * image = bmp24_loadImage("lena_color.bmp");
    bmp24_equalize(image);
    bmp24_saveImage(image, "testequa.bmp");
    */

    return 0;
}



