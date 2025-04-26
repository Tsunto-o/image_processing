#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "bmp8.c"

int main() {
    float *kernel[3];
    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = 1.0f / 9.0f;
        }
    }

    t_bmp8 * test = bmp8_loadImage("barbara_gray.bmp");
    bmp8_printInfo(test);
    bmp8_applyFilter(test, kernel, 3);
    bmp8_saveImage("test.bmp", test);
    //printf("%d", test->height);

    return 0;
}