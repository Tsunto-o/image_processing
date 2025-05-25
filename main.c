#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_intsup.h>

#include "bmp24.c"
#include "bmp8.c"
#include "menu.c"

int main() {
    menu();

    //t_bmp24 * test24 = bmp24_loadImage("lena_color.bmp");
    //bmp24_negative(test24);
    //bmp24_saveImage(test24, "test2.bmp");

    return 0;
}



