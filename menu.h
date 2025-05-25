//
// Created by tsunt on 25/05/2025.
//

#ifndef MENU_H
#define MENU_H

#endif //MENU_H


float **Allocate_Kernel();
/* allocation mémoire pour tableau kernel */

void freeKernel(float **kernel, int size);
/* libère mémoire allouée pour kernel */

void bmp24_printInfo(t_bmp24 *image);
/* affiche les informations sur le fichier */

int menu();
/* Gestion de l'interface */
