#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include "png.cpp"

// Le contexte de notre bruit. Pour ne pas être empilé à chaque appel recursif on stocke les informations
// utiles à notre fonction de bruit dans une structure.
// Sauf que c'est une mauvaise idée en fait...
// Chaque accès au membre de la structure nécessite un déréférencement
//
//http://stackoverflow.com/questions/2926062/c-using-a-lot-of-structs-can-make-a-program-slow 

typedef struct _NOISE_CONTEXT {
  int x;
  int y;

  // Décalage du tampon
  int randX;
  int randY;

  // Diviseur pour diminuer l'influence du bruit local en fonction de l'octave courrante
  int inc;

  // Coordonnées locales au tampon courant
  int stampX;
  int stampY;

  // Nouvelles Coordonnées si dépassement du tampon en dehors de la heightmap
  int wrapX;
  int wrapY;

  int tmpCoordX;
  int tmpCoordY;
  double tmpStamp;

  // Détermine le signe du tampon.
  // S'il est positif, le terrain se surélève, à l'inverse, il se creuse
  float sign;

  // Matrice contenant le tampon
  double * stamp;

  // Matrice contenant la heightmap
  double * matrix;

  // Dimension réelle de la heightmap
  int scale;

  // Demi dimension courante
  int halfScale;
} NOISE_CONTEXT;


// Le générateur de nombre pseudo aléatoire.
unsigned long int getRandom() {
        timespec tStruct;
        clock_gettime(CLOCK_REALTIME, &tStruct);
        return tStruct.tv_nsec;
}

// Le générateur de tampon
void UNIVERSE_STAMP_1( double * matrix, int scale) {
	unsigned int x,y;
	double halfScale = (double) scale / 2 ;
	double radius;
        int limit = scale*scale;

        // Optimisation sympa, plutôt que d'incrémenter d'une unité x et calculer la position du point courant 
        // dans le tableau en multipliant par scale, on incrémente directement x par scale. La formule
        // pour retrouver le point courant n'est plus 
        // 
        // x*scale +y 
        //
        // mais 
        //
        // x+y
	for(x=0;x<scale;x+=scale) {
		for(y=0;y<scale;y++) {
                        // On calcule le rayon du cercle sur lequel se trouve le point courant.
                        // Opération très TRÈS gourmante en temps CPU
			radius = sqrtl( ((y-halfScale) * (y-halfScale)) + ((x-halfScale) * (x-halfScale)));

			if ( radius < halfScale ) {
                                // y a plus qu'à dessiner le cône.
				matrix[x+y] = (halfScale - radius) / (halfScale);
			}
                        // Si on est en dehors du cercle, on se casse pas la tête et on affecte un zéro.
			else {
				matrix[x+y] = 0;
			}
		}
	}
}

void UNIVERSE_STAMP_NOISE(NOISE_CONTEXT * context, int scale, int offsetX, int offsetY) {

        // La condition d'arrêt de notre bruit récursif.
        // Selon la granularité que l'on désire, on peut augmenter la valeur limite de scale.
        if (scale == 1) {
		return;
	}

        // Comme scale est une puissance de deux, plutôt que de diviser, on opère une rotation binaire.
	context->halfScale = scale >> 1;

        // Deux variables très importantes, ce sont elles qui déterminent ou sera appliqué le tampon.
        // C'est le positionnement aléatoire qui fait toute la "beauté" de la heightmap.
	context->randX = - context->halfScale + getRandom() % scale;
	context->randY = - context->halfScale + getRandom() % scale;

        // À chaque octave il faut diminuer l'influence du bruit.
	context->inc = context->scale / scale;

        // Deux variables incrémentales qui servent à récupérer le pixel local au tampon, en fonction de l'ocatve.
        // Elles sont toutes les deux incrémentées avec la valeur de inc.
	context->stampX=0;
        context->stampY=0;

	context->sign = getRandom() & 1 ? -1.0 : 1.0;
        for(context->x=0; context->x < scale; context->x++) {
          context->stampY = 0;
	  for(context->y=0; context->y < scale; context->y++) {

            // On économise des calculs fastidieux en stockant cette valeur qui sera solicitée au moins une fois.
            context->tmpStamp = context->stamp[context->stampX + context->scale*context->stampY];

            // Avec ce test le gros bloc d'instructions est répété 1.27 fois moins que s'il n'y avait pas eu de test.
            if( context->tmpStamp != 0 ) {
              // On économise des calculs fastidieux en stockant ces valeurs qui seront solicitées plusieurs fois.
              context->tmpCoordX = context->randX + offsetX + context->x;
              context->tmpCoordY = context->randY + offsetY + context->y;

              // Le cas simple où le tampon ne dépasse pas de la heightmap
	      if ( context->tmpCoordX < context->scale && context->tmpCoordX >= 0 && context->tmpCoordY < context->scale &&  context->tmpCoordY >= 0) {
	        context->matrix[ ((context->tmpCoordX) * context->scale) + context->tmpCoordY] += context->sign * context->stamp[context->stampX + context->scale*context->stampY] / context->inc;
	      }
	      // Là c'est plus pénible, il faut calculer le décalage à appliquer selon le ou les côtés où le pixel à dépassé.
	      else {
                // On restaure les coordonnées du décalage
                // Comme il se peut que le pixel ne dépasse que sur un axe, par défaut, le décalage est fixé à zéro.
	        context->wrapX = 0;
	        context->wrapY = 0;

                // Le pixel dépasse à droite
      	        if ( context->tmpCoordX >= context->scale && context->tmpCoordX < context->scale*2 ) {
	          context->wrapX = - context->scale;
	        }
                // Le pixel dépasse à gauche
	        else if ( context->tmpCoordX > -context->scale && context->tmpCoordX < 0) {
	          context->wrapX = context->scale;
	        }

                // Le pixel dépasse en haut
                if ( context->tmpCoordY > -context->scale && context->tmpCoordY < 0) {
	          context->wrapY = context->scale;
	        }
                // Le pixel dépasse en bas
	        else if ( context->tmpCoordY < context->scale * 2 && context->tmpCoordY >= context->scale) {
	          context->wrapY = -context->scale;
	        }

                // On peut maintenant repositionner le pixel sur la heightmap.
                // la coordoonée finale dans un tableau simulant une matrice est de la forme:
                //
                // (X * hauteur) + Y
                // Avec X valant la somme du 
                //  décalage du secteur courant en abscisse, 
                //  du décalage du tampon courant en abscisse,
                //  la coordonnée x courante,
                //  le décalage aléatoire en abscisse
                //
                // Avec Y valant la somme du 
                //  décalage du secteur courant en ordonnée, 
                //  du décalage du tampon courant en ordonnée,
                //  la coordonnée y courante,
                //  le décalage aléatoire en ordonnée
                context->matrix[ (context->wrapX + context->tmpCoordX) * context->scale + context->tmpCoordY+ context->wrapY] += context->sign * context->tmpStamp / context->inc;
              }
            }
            // On incrémente à l'échelle la coordonnée locale au tampon
            context->stampY += context->inc;
	  }
          // On incrémente à l'échelle la coordonnée locale au tampon
	  context->stampX += context->inc;
	}
	

        // En divisant par deux la dimension courante à chaque récursion, et en modifiant l'offset,
        // on subdivise en permanence la heighmap jusqu'à ce que la dimension ainsi divisée soit égale à un.
        // En procédant ainsi, on travaille récursivement sur différentes
        // portions de la heightmap. Il y a donc quatre portions par secteur et à chaque récursion, chacune
        // des portions devient elle-même un secteur.

        // Portion en haut à gauche du secteur courant
	UNIVERSE_STAMP_NOISE(context, scale/2, offsetX+0, offsetY+0);

        // Portion en bas à gauche du secteur courant
	UNIVERSE_STAMP_NOISE(context, scale/2, offsetX+0, offsetY+scale/2);
       
        // Portion en bas à droite du secteur courant
	UNIVERSE_STAMP_NOISE(context, scale/2, offsetX+scale/2, offsetY+scale/2);

        // Portion en haut à droite du secteur courant
	UNIVERSE_STAMP_NOISE(context, scale/2, offsetX+scale/2, offsetY+0);
}

int main(int argc, char ** argv) {
	if (argc == 1) {
		std::cout << "Usage: ./heightMap <power of two>\n";
		return 0;
	}

        NOISE_CONTEXT context;
        context.scale = atoi(argv[1]);
	unsigned int x,y,i,j;
	context.matrix	= (double *) malloc( sizeof( double ) * context.scale * context.scale);
	context.stamp 	= (double *) malloc( sizeof( double *) * context.scale * context.scale);
        PIXEL ** png = (PIXEL **) malloc(sizeof(PIXEL *) * context.scale);

        // On s'assure que la matrice de destination sera bien "vierge"
	for (x=0; x<context.scale;x++) {
		for(y=0;y<context.scale;y++){
			context.matrix[x*context.scale+y] = 0;
		}
		png[x] = (PIXEL *) malloc(sizeof(PIXEL) * context.scale);
	}

        // On génère d'abord notre tampon
	UNIVERSE_STAMP_1(context.stamp, context.scale);

        // On lance notre bruit récursif.
        // On conmmence la récursion avec l'octave la plus grande.
	UNIVERSE_STAMP_NOISE(&context, context.scale, 0, 0);

        // À partir d'ici, la heightmap est terminée. Il n'y a plus qu'a déterminer les extremums
        // pour normaliser la hauteur.
        double max= -2,min = 2;
        for (x=0; x<context.scale;x++) {
                for(y=0;y<context.scale;y++) {
                        if (context.matrix[x*context.scale+y] > max) {
                                max = context.matrix[x*context.scale+y];
                        }
                        if (context.matrix[x*context.scale+y] < min) {
                                min = context.matrix[x*context.scale+y];
                        }
                }
        }

        char color;
        for (x=0; x<context.scale;x++) {
                for(y=0;y<context.scale;y++) {
                        color = (((context.matrix[x*context.scale+y]) - min ) * 255) / (max - min);
                        png[x][y].Alpha = 0xFF;
                        png[x][y].Red = color;
                        png[x][y].Green = color;
                        png[x][y].Blue = color;
                }
        }

        // On transfére notre heightmap dans un fichier png...
        writePng(png,context.scale);
	return 0;
}
