#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "bmp24.c"
#include "bmp8.c"

int main() {
    /*
    //Kernel de Box Blur
    float *box_blur[3];
    for (int i = 0; i < 3; i++) {
        box_blur[i] = (float *)malloc(sizeof(float) * 3);
    }
    box_blur[0][0] = (float)1/9; box_blur[0][1] = (float)1/9; box_blur[0][2] = (float)1/9;
    box_blur[1][0] = (float)1/9; box_blur[1][1] = (float)1/9; box_blur[1][2] = (float)1/9;
    box_blur[2][0] = (float)1/9; box_blur[2][1] = (float)1/9; box_blur[2][2] = (float)1/9;


    //Kernel de Gaussian Blur
    float *gaussian_blur[3];
    for (int i = 0; i < 3; i++) {
        gaussian_blur[i] = (float *)malloc(sizeof(float) * 3);
    }
    gaussian_blur[0][0] = (float)1/16; gaussian_blur[0][1] = (float)2/16; gaussian_blur[0][2] = (float)1/16;
    gaussian_blur[1][0] = (float)2/16; gaussian_blur[1][1] = (float)4/16; gaussian_blur[1][2] = (float)2/16;
    gaussian_blur[2][0] = (float)1/16; gaussian_blur[2][1] = (float)2/16; gaussian_blur[2][2] = (float)1/16;


    //Kernel de Outline
    float *outline[3];
    for (int i = 0; i < 3; i++) {
        outline[i] = (float *)malloc(sizeof(float) * 3);
    }
    outline[0][0] = -1; outline[0][1] = -1; outline[0][2] = -1;
    outline[1][0] = -1; outline[1][1] = 8; outline[1][2] = -1;
    outline[2][0] = -1; outline[2][1] = -1; outline[2][2] = -1;


    //Kernel de Emboss
    float *emboss[3];
    for (int i = 0; i < 3; i++) {
        emboss[i] = (float *)malloc(sizeof(float) * 3);
    }
    emboss[0][0] = -2; emboss[0][1] = -1; emboss[0][2] = 0;
    emboss[1][0] = -1; emboss[1][1] = 1; emboss[1][2] = 1;
    emboss[2][0] = 0; emboss[2][1] = 1; emboss[2][2] = 2;


    //Kernel de Sharpen
    float *sharpen[3];
    for (int i = 0; i < 3; i++) {
        sharpen[i] = (float *)malloc(sizeof(float) * 3);
    }
    sharpen[0][0] = 0; sharpen[0][1] = -1; sharpen[0][2] = 0;
    sharpen[1][0] = -1; sharpen[1][1] = 5; sharpen[1][2] = -1;
    sharpen[2][0] = 0; sharpen[2][1] = -1; sharpen[2][2] = 0;



    t_bmp8 * test = bmp8_loadImage("C:/Users/Gia-Vinh/Desktop/image_processing/barbara_gray.bmp");
    bmp8_printInfo(test);

    bmp8_applyFilter(test, sharpen, 3);
    for (int i = 0; i < 3; i++) {
        free(sharpen[i]);
    }
    bmp8_saveImage("C:/Users/Gia-Vinh/Desktop/image_processing/sharpen.bmp", test);
    */

    t_bmp24 * test24 = bmp24_loadImage("barbara_gray.bmp");
    printf("%x", test24->data);
    //bmp24_saveImage("test.bmp", test24);

    return 0;
}



