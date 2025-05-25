//
// Created by tsunt on 25/05/2025.
//

#include "menu.h"
#include <stdio.h>



#include <stdio.h>

float **Allocate_Kernel() {
    float **kernel = malloc(3 * sizeof(float *));
    if (!kernel) {
        perror("Erreur d'allocation du tableau de lignes");
        return NULL;
    }

    for (int i = 0; i < 3; i++) {
        kernel[i] = malloc(3 * sizeof(float));
        if (!kernel[i]) {
            perror("Erreur d'allocation d'une ligne du noyau box blur");
            // Libération partielle en cas d'erreur
            for (int k = 0; k < i; k++) free(kernel[k]);
            free(kernel);
            return NULL;
        }
    }
    return kernel;
}


void freeKernel(float **kernel, int size) {
    if (!kernel) return;

    for (int i = 0; i < size; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

#include <stdio.h>

void bmp24_printInfo(t_bmp24 *image) {
    if (!image) {
        printf("Image invalide (NULL)\n");
        return;
    }

    printf("\n=== En-tête BMP (BITMAP FILE HEADER) ===\n");
    printf("Type: 0x%X\n", image->header.type);
    printf("Taille du fichier: %u octets\n", image->header.size);
    printf("Réservé1: %u\n", image->header.reserved1);
    printf("Réservé2: %u\n", image->header.reserved2);
    printf("Offset des données: %u\n", image->header.offset);

    printf("\n=== En-tête Info BMP (BITMAP INFO HEADER) ===\n");
    printf("Taille de l'en-tête info: %u octets\n", image->header_info.size);
    printf("Largeur: %d px\n", image->header_info.width);
    printf("Hauteur: %d px\n", image->header_info.height);
    printf("Plans de couleur: %u\n", image->header_info.planes);
    printf("Bits par pixel: %u\n", image->header_info.bits);
    printf("Compression: %u\n", image->header_info.compression);
    printf("Taille de l'image (données): %u octets\n", image->header_info.imagesize);
    printf("Résolution horizontale: %d px/m\n", image->header_info.xresolution);
    printf("Résolution verticale: %d px/m\n", image->header_info.yresolution);
    printf("Couleurs dans la palette: %u\n", image->header_info.ncolors);
    printf("Couleurs importantes: %u\n", image->header_info.importantcolors);

    printf("\n=== Informations supplémentaires ===\n");
    printf("Largeur (copie): %d px\n", image->width);
    printf("Hauteur (copie): %d px\n", image->height);
    printf("Profondeur de couleur (copie): %d bits\n", image->colorDepth);
}












int menu() {
    int choix, brightness;
    char cheminFichier[256];
    do {
        printf("Veuillez choisir une option :\n");
        printf("1. Ouvrir une image\n");
        printf("2. Sauvegarder une image\n");
        printf("3. Appliquer un filtre\n");
        printf("4. Afficher les informations de l'image\n");
        printf("5. Quitter\n");
        printf("Votre choix : ");
        scanf(" %d", &choix);

        switch (choix) {
            case 1: //Ouvrir une image
                printf("Chemin du fichier : ");
                getchar(); // vider le tampon
                fgets(cheminFichier, sizeof(cheminFichier), stdin);
                cheminFichier[strcspn(cheminFichier, "\n")] = '\0'; // Retirer le retour à la ligne s'il est présent
                t_bmp24 * image = bmp24_loadImage(cheminFichier);
                //printf("%d", image->header.offset);
            break;

            case 2:
                if (!image) {
                    printf("Aucune image à sauvegarder.\n");
                }else {
                    getchar(); //nettoyer buffer après scanf
                    printf("Chemin du fichier : ");
                    fgets(cheminFichier, sizeof(cheminFichier), stdin);
                    cheminFichier[strcspn(cheminFichier, "\n")] = '\0'; // Retirer le retour à la ligne s'il est présent
                    bmp24_saveImage(image, cheminFichier);
                }
            // Code pour sauvegarder une image ici
            break;

            case 3:
                if (!image) {
                    printf("Aucune image chargée.\n");
                    break;
                }
                int filtreChoix;
                do {
                    printf("\nVeuillez choisir un filtre :\n");
                    printf("1. Négatif\n");
                    printf("2. Luminosité\n");
                    printf("3. Binarisation\n");
                    printf("4. Flou\n");
                    printf("5. Flou gaussien\n");
                    printf("6. Netteté\n");
                    printf("7. Contours\n");
                    printf("8. Relief\n");
                    printf("9. Retourner au menu précédent\n");
                    printf("Votre choix : ");

                    if (scanf("%d", &filtreChoix) != 1) {
                        getchar(); // Nettoyer le \n après scanf
                        printf("Entrée invalide.\n");
                        while (getchar() != '\n');
                        continue;
                    }
                    getchar(); // Nettoyer le \n après scanf

                    switch (filtreChoix) {
                        case 1:
                            bmp24_negative(image);
                            printf("Filtre Négatif appliqué.\n"); break;


                        case 2:
                            printf("Entrez une valeur entière : ");
                            scanf(" %d", &brightness);
                            getchar();
                            bmp24_brightness(image, brightness);
                            printf("Filtre Luminosité appliqué.\n"); break;


                        case 3: printf("Filtre Binarisation appliqué.\n"); break;
                            bmp24_grayscale(image);
                            printf("Filtre Binarisation appliqué.\n"); break;


                        case 4:
                            //kernel box blur
                            float **box_blur = Allocate_Kernel();
                            box_blur[0][0] = (float)1/9; box_blur[0][1] = (float)1/9; box_blur[0][2] = (float)1/9;
                            box_blur[1][0] = (float)1/9; box_blur[1][1] = (float)1/9; box_blur[1][2] = (float)1/9;
                            box_blur[2][0] = (float)1/9; box_blur[2][1] = (float)1/9; box_blur[2][2] = (float)1/9;

                            //application filtre box blur
                            bmp24_applyFilter(image, box_blur, 3);
                            printf("Filtre Flou appliqué.\n");

                            //libération kernel
                            freeKernel(box_blur, 3);
                            break;


                        case 5:
                            float **gaussian_blur = Allocate_Kernel();
                            gaussian_blur[0][0] = (float)1/16; gaussian_blur[0][1] = (float)2/16; gaussian_blur[0][2] = (float)1/16;
                            gaussian_blur[1][0] = (float)2/16; gaussian_blur[1][1] = (float)4/16; gaussian_blur[1][2] = (float)2/16;
                            gaussian_blur[2][0] = (float)1/16; gaussian_blur[2][1] = (float)2/16; gaussian_blur[2][2] = (float)1/16;

                            //application filtre gaussian_blur
                            bmp24_applyFilter(image, gaussian_blur, 3);
                            printf("Filtre Flou gaussien appliqué.\n");

                            //libération kernel
                            freeKernel(gaussian_blur, 3);
                            break;


                        case 6:
                            //kernel sharpen
                            float **sharpen = Allocate_Kernel();
                            sharpen[0][0] = 0; sharpen[0][1] = -1; sharpen[0][2] = 0;
                            sharpen[1][0] = -1; sharpen[1][1] = 5; sharpen[1][2] = -1;
                            sharpen[2][0] = 0; sharpen[2][1] = -1; sharpen[2][2] = 0;

                            //application filtre sharpen
                            bmp24_applyFilter(image, sharpen, 3);
                            printf("Filtre Netteté appliqué.\n");

                            //libération kernel
                            freeKernel(sharpen, 3);
                            break;


                        case 7:
                            //kernel outline
                            float **outline = Allocate_Kernel();
                            outline[0][0] = -1; outline[0][1] = -1; outline[0][2] = -1;
                            outline[1][0] = -1; outline[1][1] = 8; outline[1][2] = -1;
                            outline[2][0] = -1; outline[2][1] = -1; outline[2][2] = -1;

                            //application filtre outline
                            bmp24_applyFilter(image, outline, 3);
                            printf("Filtre Contours appliqué.\n");

                            //libération kernel
                            freeKernel(outline, 3);
                            break;


                        case 8:
                            //kernel emboss
                            float **emboss = Allocate_Kernel();
                            emboss[0][0] = -2; emboss[0][1] = -1; emboss[0][2] = 0;
                            emboss[1][0] = -1; emboss[1][1] = 1; emboss[1][2] = 1;
                            emboss[2][0] = 0; emboss[2][1] = 1; emboss[2][2] = 2;

                            //application filtre emboss
                            bmp24_applyFilter(image, emboss, 3);
                            printf("Filtre Relief appliqué.\n");

                            //libération kernel
                            freeKernel(emboss, 3);
                            break;


                        case 9: printf("Retour au menu principal.\n"); break;
                        default: printf("Choix invalide.\n");
                    }
                    printf("\n");

                } while (filtreChoix != 9);
            break;

            case 4:
                bmp24_printInfo(image);

            break;
            case 5:
                printf("Arrêt du programme.\n");
            break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
                getchar();

        }

        printf("\n");

    } while (choix != 5);

    return 0;
}
