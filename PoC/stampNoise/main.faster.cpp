#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include "png.h"

// Le générateur de nombres pseudo aléatoires. Dépend de la plateforme.

// Pour Windows
#ifdef _WIN32
#include <ctime>

unsigned long int getRandom(int salt){
    std::srand(std::time(0)); // use current time as seed for random generator
    return std::rand()+salt*3.1415;
}

// Pour Posix/Linux
#else
unsigned long int getRandom(int salt) {
        timespec tStruct;
        clock_gettime(CLOCK_REALTIME, &tStruct);
        return tStruct.tv_nsec;
}

#endif

// Le générateur de tampon
void UNIVERSE_STAMP_1(double * matrix, int scale) {
	int x,y,X=0;
	double halfScale = scale / 2 ;
	double radius;

        int * powersOfTwo = (int *) malloc( sizeof(int) * scale);

        // On crée deux tables contenant les valeurs élevées à la puissance de deux.
        // On calcule ainsi n fois ces valeurs au lieu de n².
        for(x=0; x<scale;x++) {
          powersOfTwo[x] = (x-halfScale) * (x-halfScale);
        }

        // Plutôt que d'incrémenter d'une unité x et calculer la position du point courant
        // dans le tableau en multipliant par scale, on incrémente directement x par scale. La formule
        // pour retrouver le point courant n'est plus
        //
        // x*scale +y
        //
        // mais
        //
        // x+y
        //
        // Il faut donc également penser à changer la valeur limite de la première boucle
        int limit = scale*scale;
	for(x=0;x<limit;x+=scale) {
		for(y=0;y<scale;y++) {
                        // On calcule le rayon du cercle sur lequel se trouve le point courant.
                        // Opération très TRÈS gourmante en temps CPU
			radius = sqrtl( powersOfTwo[y] + powersOfTwo[X]);
			if ( radius < halfScale ) {
                                // y a plus qu'à dessiner le cône.
				matrix[x+y] = (halfScale - radius) / (halfScale);
			}
			else {
                                // Si on est en dehors du cercle, on se casse pas la tête et on affecte un zero.
				matrix[x+y] = 0;
			}
		}
                //Comme x est incrémenté par scale, et qu'on doit quand même accéder à notre tableau powersOfTwo...
                X++;
	}
        free(powersOfTwo);
}

void UNIVERSE_STAMP_NOISE(double * matrix, double * stamp, int scale, int offsetX, int offsetY, int realScale) {
        // La condition d'arrêt de notre bruit récursif.
        // Selon la granularité que l'on désire, on peut augmenter la valeur limite de scale.
        if (scale == 1) {
		return;
	}

        // Demi dimension courante
        // Comme scale est une puissance de deux, plutôt que de diviser, on opère une rotation binaire.
	int halfScale = scale >> 1;
	int x, y;

        // Deux variables très importantes, ce sont elles qui déterminent où sera appliqué le tampon.
        // C'est le positionnement aléatoire qui fait toute la "beauté" de la heightmap.
	int randX = - halfScale + getRandom(offsetX - offsetY) % scale;
	int randY = - halfScale + getRandom(offsetX + offsetY) % scale;

        // À chaque octave il faut diminuer l'influence du bruit.
	// On se sert également de cette variable comme pas d'incrémentation des
        // coordonnées du tampon.
        int inc = realScale / scale;

        // Deux variables incrémentales qui servent à récupérer le pixel local au tampon, en fonction de l'octave.
        // Elles sont toute les deux incrémentés avec la valeur de inc.
	int stampX=0, stampY=0;

        // Nouvelles coordonnées si dépassement du tampon en dehors de la heightmap
	int wrapX,wrapY;

        // Détermine le signe du tampon.
        // S'il est positif, le terrain se surélève, à l'inverse, il se creuse
	float sign = getRandom(offsetX ^ offsetY) & 2 ? -1.0 : 1.0;

        int tmpCoordX,tmpCoordY;
        double currentStampValue;

	for(x=0;x<scale;x++) {
	  stampY = 0;
	    for(y=0;y<scale;y++) {
              // On économise des calculs fastidieux en stockant cette valeur qui sera solicitée au moins une fois.
              currentStampValue = stamp[stampX*realScale+stampY];

              // Avec ce test le gros bloc d'instructions est répété 1.27 fois moins que s'il n'y avait pas de test.
              if (currentStampValue != 0) {
                // On économise des calculs fastidieux en stockant ces valeurs qui seront solicitées plusieurs fois.
                tmpCoordX = randX + offsetX + x;
                tmpCoordY = randY + offsetY + y;

                // Le cas simple où le tampon ne dépasse pas de la heightmap
	        if ( tmpCoordX < realScale && tmpCoordX >= 0 && tmpCoordY < realScale && tmpCoordY >= 0) {
	          matrix[ (tmpCoordX * realScale) + tmpCoordY] += sign * currentStampValue / (inc);
	        }

	        // Là c'est plus pénible, il faut calculer le décalage à appliquer selon le ou les côtés où le pixel à dépassé.
	        else {
                  // On restaure les coordonnées du décalage
                  // Comme il se peut que le pixel ne dépasse que sur un axe, par défaut, le décalage est fixé à zéro.
		  wrapX = 0;
	    	  wrapY = 0;
                  // Le pixel dépasse à droite
		  if ( tmpCoordX >= realScale && tmpCoordX < realScale*2 ) {
      		    wrapX = - realScale;
		  }

                  // Le pixel dépasse à gauche
		  else if ( tmpCoordX > -realScale && tmpCoordX < 0) {
		    wrapX = realScale;
	  	  }

                  // Le pixel dépasse en haut
		  if ( tmpCoordY > -realScale && tmpCoordY < 0) {
    		    wrapY = realScale;
		  }

                  // Le pixel dépasse en bas
		  else if ( tmpCoordY < realScale * 2 && tmpCoordY >= realScale) {
		    wrapY = -realScale;
		  }

                  // On peut maintenant repositionner le pixel sur la heightmap.
                  // la coordoonée finale dans un tableau simulant une matrice est de la forme:
                  //
                  // (X * hauteur) + Y
                  // Avec X valant la somme du
                  //  décalage du secteur courant en abcisse,
                  //  du décalage du tampon courant en abcisse,
                  //  la coordonnée x courante,
                  //  le décalage aléatoire en abcisse
                  //
                  // Avec Y valant la somme du
                  //  décalage du secteur courant en ordonnée,
                  //  du décalage du tampon courant en ordonnée,
                  //  la coordonnée y courante,
                  //  le décalage aléatoire en ordonnée
		  matrix[ (  wrapX + tmpCoordX) * realScale + tmpCoordY + wrapY] += sign * currentStampValue / (inc);
      	        }
              }
              // On incrémente à l'échelle la coordonnée locale au tampon
	      stampY+=inc;
	    }
          // On incrémente à l'échelle la coordonnée locale au tampon
	  stampX+=inc;
	}

        // En divisant par deux la dimension courante à chaque récursion, et en modifiant l'offset,
        // on subdivise en permanence la heightmap jusqu'à ce que la dimension ainsi divisée soit égale à un.
        // En procédant ainsi, on travaille récursivement sur différentes
        // portions de la heighmap. Il y a donc quatre portions par secteur et à chaque récursion, chacune
        // des portions devient elle-même un secteur.

        // Portion en haut à gauche du secteur courant
	UNIVERSE_STAMP_NOISE(matrix, stamp, scale/2, offsetX+0, offsetY+0, realScale);

        // Portion en bas à gauche du secteur courant
	UNIVERSE_STAMP_NOISE(matrix, stamp, scale/2, offsetX+0, offsetY+scale/2, realScale);

        // Portion en bas à droite du secteur courant
        UNIVERSE_STAMP_NOISE(matrix, stamp, scale/2, offsetX+scale/2, offsetY+scale/2, realScale);

        // Portion en haut à droite du secteur courant
	UNIVERSE_STAMP_NOISE(matrix, stamp, scale/2, offsetX+scale/2, offsetY+0, realScale);
}

int main(int argc, char ** argv) {

	if (argc == 1) {
		std::cout << "Usage: ./heightMap <power of two>\n";
		return 0;
	}

	int scale = atoi(argv[1]);
	int x,y,i,j;
	double * matrix;
	double * stamp;
	matrix	= (double *) malloc( sizeof( double ) * scale * scale);
	stamp 	= (double *) malloc( sizeof( double ) * scale * scale);
        PIXEL ** png = (PIXEL **) malloc(sizeof(PIXEL *) * scale);

        // On s'assure que la matrice de destination sera bien "vierge"
	for (x=0; x<scale;x++) {
		for(y=0;y<scale;y++){
			matrix[x*scale+y] = 0;
		}
		png[x] = (PIXEL *) malloc(sizeof(PIXEL) * scale);
	}

        // On génère d'abord notre tampon
	UNIVERSE_STAMP_1(stamp, scale);

        // On lance notre bruit récursif.
        // On conmmence la récursion avec l'octave la plus grande.
	UNIVERSE_STAMP_NOISE(matrix, stamp, scale, 0, 0, scale);

        // À partir d'ici, la heightmap est terminée. Il n'y a plus qu'à déterminer les extremums
        // pour normaliser la hauteur.

        long double max=0,min = 65536;
        for (x=0; x<scale;x++) {
                for(y=0;y<scale;y++) {
                        if (matrix[x*scale+y] > max) {
                                max = matrix[x*scale+y];
                        }
                        if (matrix[x*scale+y] < min) {
                                min = matrix[x*scale+y];
                        }
                }
        }

        char color;
        for (x=0; x<scale;x++) {
                for(y=0;y<scale;y++) {
                        color = (((matrix[x*scale+y]) - min ) * 255) / (max - min);
                        png[x][y].Alpha = 0xFF;
                        png[x][y].Red = color;
                        png[x][y].Green = color;
                        png[x][y].Blue = color;
                }
        }

        // On transfère notre heightmap dans un fichier png...
        writePng(png,scale);
	return 0;
}
