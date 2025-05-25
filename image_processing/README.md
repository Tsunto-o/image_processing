# BMP 24-bit Image Processing

Ce projet en langage C permet de charger, modifier et sauvegarder des images au format BMP 24 bits.

## Auteurs
- TO Gia (étudiant à EFREI Paris)
- SCHOUAMES Enzo (étudiant à EFREI Paris)


## Fonctionnalités

- Chargement d’une image BMP 24 bits (`bmp24_loadImage`)
- Sauvegarde d’une image modifiée (`bmp24_saveImage`)
- Affichage des données de l'image
- Filtres d’image :
  - **Négatif** : inversion des couleurs (`bmp24_negative`)
  - **Binarisation** : conversion en noir et blanc (`bmp24_grayscale`)
  - **Luminosité** : ajustement de l’intensité lumineuse (`bmp24_brightness`)
  - **Flou** 
  - **Flou Gaussien** 
  - **Netteté** 
  - **Contours**
  - **Relief**



## Utilisation

Le programme est déjà compilé. Il suffit d'exécuter le fichier `main.exe`. 



## Compilation

Si toutefois vous voulez compiler le code pour l'exécuter, utilisez `gcc` pour compiler :

```bash
gcc -o main.exe main.c
