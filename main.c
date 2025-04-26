#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "bmp8.c"

int main() {
    t_bmp8 * test = bmp8_loadImage("barbara_gray.bmp");
    bmp8_saveImage("testsave.bmp", test);
    //printf("%d", test->height);

    return 0;
}