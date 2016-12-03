#include <iostream>
#include <cmath>
#include "../common.hpp"
#include "cubeMap.hpp"

CubeMap::CubeMap(int LOD) {
  this->scale = (2 << (LOD - 1));
  int area = this->scale*this->scale;
  this->stamp = new float[area];

  std::cout << "Initiate six heightmaps of " << this->scale << " pixels²...\n";
  // On initialise les six faces du cube.
  for (int i = 0; i<6; i++) {
    this->faces[i]	= new float[area];
  }

  // On fait en sorte que chaque face soit vierge
  for(int y = 0; y<area; y+= this->scale) {
    for(int x = 0; x<this->scale; x++) {
      for (int i = 0; i<6; i++) {
        this->faces[i][x + y] = 0;
      }
    }
  }
  
  // On crée un tampon
  this->Stamp();

  // Pour chaque face qu'on va traiter, il faut déterminer les faces voisines.

  this->top    = this->faces[2];
  this->bottom = this->faces[3];
  this->left   = this->faces[5];
  this->right  = this->faces[4];
  this->Noise(this->scale, 0, 0, 0);

  this->top    = this->faces[3];
  this->bottom = this->faces[2];
  this->left   = this->faces[5];
  this->right  = this->faces[4];
  this->Noise(this->scale, 0, 0, 1);

  this->top     = this->faces[1];
  this->bottom  = this->faces[0];
  this->left    = this->faces[5];
  this->right   = this->faces[4];
  this->Noise(this->scale, 0, 0, 2);

  this->top     = this->faces[0];
  this->bottom  = this->faces[1];
  this->left    = this->faces[5];
  this->right   = this->faces[4];

  this->Noise(this->scale, 0, 0, 3);

  this->top     = this->faces[2];
  this->bottom  = this->faces[3];
  this->left    = this->faces[0];
  this->right   = this->faces[1];
  this->Noise(this->scale, 0, 0, 4);

  this->top     = this->faces[2];
  this->bottom  = this->faces[3];
  this->left    = this->faces[1];
  this->right   = this->faces[0];
  this->Noise(this->scale, 0, 0, 5);

  // Cela étant fait il faut racorder chacune des faces. En effet, en l'état, 
  // si les heighmaps sont appliquées à la sphére projeté il y aura des défauts
  // de jointures entre chacunes des faces du cube.

  this->top    = this->faces[2];
  this->bottom = this->faces[3];
  this->left   = this->faces[5];
  this->right  = this->faces[4];
  this->MergeEdges(0);

  this->top    = this->faces[3];
  this->bottom = this->faces[2];
  this->left   = this->faces[5];
  this->right  = this->faces[4];
  this->MergeEdges(1);

  // Maintenant que nous avons calculé deux faces opposées, tous les sommets sont bon.
  // Huits segments sur douze sont également calculés.

  this->top     = this->faces[1];
  this->bottom  = this->faces[0];
  this->left    = this->faces[5];
  this->right   = this->faces[4];
  this->MergeEdges(2);

  this->top     = this->faces[0];
  this->bottom  = this->faces[1];
  this->left    = this->faces[5];
  this->right   = this->faces[4];
  this->MergeEdges(3);
};

void CubeMap::Stamp() {
  int x,y,X=0;
  double halfScale = this->scale / 2 ;
  double radius;

  //int * powersOfTwo = (int *) malloc( sizeof(int) * this->scale);
  int * powersOfTwo = new int[this->scale];
  // On crée deux tables contenants les valeurs élevées à la puissance de deux.
  // On calcule ainsi n fois ces valeurs au lieu de n².
  for(x=0; x<this->scale;x++) {
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
  int limit = this->scale*this->scale;
  for(x=0;x<limit;x+=this->scale) {
    for(y=0;y<this->scale;y++) {
      // On calcule le rayon du cercle sur lequel se trouve le point courant.
      // Opération très TRÈS gourmande en temps CPU
      radius = sqrtl( powersOfTwo[y] + powersOfTwo[X]);
      if ( radius < halfScale ) {
        // y a plus qu'à dessiner le cône.
	this->stamp[x+y] = (halfScale - radius) / (halfScale);
      }
      else {
        // Si on est en dehors du cercle, on se casse pas la tête et on affecte un zéro.
	this->stamp[x+y] = 0;
      }
    }
    //Comme x est incrémenté par scale, et qu'on doit quand même accéder à notre tableau powersOfTwo...
    X++;
  }
  delete [] powersOfTwo; 
  //free(powersOfTwo);
}

void CubeMap::Noise( int scale, int offsetX, int offsetY, int faceId) {
  if (scale == 2) {
    return;
  }

  char seed = getRandom() & 3;
  char doIt = getRandom() & 1;
  float sign = getRandom() & 2 ? 1.0 : -1.0;
  int halfScale = scale >> 1;
  int randX = - halfScale + getRandom() % scale;
  int randY = - halfScale + getRandom() % scale;
  int inc = this->scale / scale;
  int stampIndex;
  int faceIndex;
  int stampX=0,stampY=0;
	

  float * dest;

  // Quand le tampon est contenu exactement à l'intérieur d'une face
  if ((randY+offsetY >= 0 && randY+offsetY+scale-1 < this->scale) && (randX+offsetX >= 0 && randX+offsetX+scale-1< this->scale)) {
    for(int y=0; y<scale;y++) {
      stampX = 0;
      for(int x=0; x<scale;x++) {
        stampIndex = stampX + scale * stampY;
	if( this->stamp[stampIndex] != 0) {
	  faceIndex = (y+randY+offsetY) * this->scale + x+randX+offsetX;
	  this->faces[faceId][faceIndex] += sign * this->stamp[stampIndex] / inc;
	  if (this->max < this->faces[faceId][ faceIndex]) {
	    this->max = this->faces[faceId][ faceIndex ];
	  }
	  if (this->min <= 0 && this->min > this->faces[faceId][ faceIndex ]) {
	    this->min = this->faces[faceId][ faceIndex ];
	  }
	}
	stampX += inc;
      }
      stampY += (inc * inc);
    }
  } 
  // Quand le tampon dépasse sur l'axe X
  else if ( !(randX+offsetX >= 0 && randX+offsetX+scale-1 < this->scale) && ( randY+offsetY >= 0 && randY+offsetY+scale-1 < this->scale  )) {
    for(int y=0; y<scale;y++) {
      stampX = 0;
      for(int x=0; x<scale;x++) {
        stampIndex = stampX + scale * stampY;
	if( this->stamp[stampIndex] != 0) {
	  if (randX+offsetX+x < 0) {
	    dest = this->left;
	    if (faceId == 1) {
	      faceIndex = ( this->scale - (y+randY+offsetY) ) * this->scale - (x+randX+offsetX)-1;
	    }
	    else if (faceId == 2) {
	      faceIndex = -(x+randX+offsetX + 1) * this->scale +(y+randY+offsetY);
	    }
	    else if (faceId == 3) {
	      faceIndex = ( this->scale + x+randX+offsetX - 1) * this->scale + this->scale - 1 - (y+randY+offsetY);
	    }
	    else if (faceId == 4) {
	      faceIndex = (y+randY+offsetY) * this->scale + this->scale + (x+randX+offsetX);
	    }
	    else if (faceId == 5) {
	      faceIndex = ( this->scale - (y+randY+offsetY)) * this->scale - (x+randX+offsetX) - 1;
	    }
	    else {
	      faceIndex = (y+randY+offsetY) * this->scale + this->scale + (x+randX+offsetX);
	    }
	  }
	  else if ( randX+offsetX+x >= this->scale ) {
	    dest = this->right;
	    if (faceId == 1) {
	      faceIndex = (this->scale - (y+randY+offsetY)) * this->scale + this->scale - ((x+randX+offsetX) - this->scale) - 1;
	    }
	    else if (faceId == 2) {
	      faceIndex = ((x+randX+offsetX) - this->scale) * this->scale + this->scale - (y+randY+offsetY);
	    }
	    else if (faceId == 3) {
	      faceIndex = ( this->scale - ((x+randX+offsetX) - this->scale) - 1) * this->scale + (y+randY+offsetY);
	    }
	    else if (faceId == 4) {
	      faceIndex = ( this->scale - (y+randY+offsetY) ) * this->scale + this->scale - ((x+randX+offsetX) - this->scale) - 1;
	    }
	    else if (faceId == 5) {
	      faceIndex = (y+randY+offsetY) * this->scale + (x+randX+offsetX) - this->scale;
            }
	    else {
	      faceIndex = (y+randY+offsetY) * this->scale + x+randX+offsetX - this->scale;
	    }
	  }
	  else {
	    dest = this->faces[faceId];
	    faceIndex = ((y+randY+offsetY)) * this->scale + x+randX+offsetX;
	  }
	  dest[ faceIndex] += sign * this->stamp[stampIndex] / inc ;
	  if (this->max < dest[ faceIndex]) {
	    this->max = dest[ faceIndex ];
	  }
	  if (this->min <= 0 && this->min > dest[ faceIndex ]) {
	    this->min = dest[ faceIndex ];
	  }
	}
	stampX += inc;
      }
      stampY += inc * inc;
    }
  }
  // Quand le tampon dépasse sur l'axe Y
  else if ((randX+offsetX >= 0 && randX+offsetX+scale-1 < this->scale) && !( randY+offsetY >= 0 && randY+offsetY+scale-1 < this->scale  )) {
    for(int y=0; y<scale;y++) {
      stampX = 0;
      for(int x=0; x<scale;x++) {
        stampIndex = stampX + scale * stampY;
	if( this->stamp[stampIndex] != 0) {
	  if (randY+offsetY+y < 0) {
	    dest = this->top;
	    if ( faceId == 4) {
	      faceIndex = ( this->scale + (y+randY+offsetY)) + this->scale * ( this->scale - (x+randX+offsetX));
	    }
	    else if ( faceId == 5) {
	      faceIndex = (x+randX+offsetX) * this->scale - 1 - y-randY-offsetY;
	    }
	    else {
	      faceIndex = (this->scale + (y+randY+offsetY)) * this->scale + x+randX+offsetX;
	    }
	  }
	  else if (randY+offsetY+y >= this->scale) {
	    dest = this->bottom;
	    if ( faceId == 4 ) {
	      faceIndex = - (y+randY+offsetY) - 1 + (this->scale * ( scale - x+randX+offsetX) );
	    }
	    else if(faceId == 5) {
	      faceIndex = (y+randY+offsetY - this->scale) + this->scale * (this->scale -x-randX-offsetX);
	    }
	    else {
	      faceIndex = (y+randY+offsetY - this->scale) * this->scale + x+randX+offsetX;
	    }
	  }
	  else {
	    dest = this->faces[faceId];
	    faceIndex = ((y+randY+offsetY)) * this->scale + x+randX+offsetX;
	  }
	  dest[ faceIndex] += sign * this->stamp[stampIndex] / inc ;
	  if (this->max < dest[ faceIndex]) {
	    this->max = dest[ faceIndex ];
	  }
	  if (this->min <= 0 && this->min > dest[ faceIndex ]) {
	    this->min = dest[ faceIndex ];
	  }
	}
	stampX += inc;
      }
      stampY += (inc * inc);
    }
  }
  this->Noise(halfScale, offsetX, 		offsetY,		faceId);
  this->Noise(halfScale, offsetX, 		offsetY+halfScale,	faceId);
  this->Noise(halfScale, offsetX+halfScale, 	offsetY+halfScale,	faceId);
  this->Noise(halfScale, offsetX+halfScale, 	offsetY,		faceId);
}

void CubeMap::MergeEdges(int faceId) {
  // On parcoure les quatres segments composants la face courante en une seule itération
  // On ne touche pas aux sommet du carré pour le moment, ils seront traités séparément.
  // En fait les sommets de la face courrante seront le résultat de la moyenne des sommets voisins
  // en un point donné.
  
  float average;
  for(int i = 1; i <this->scale - 1; i++ ) {
    if(faceId == 0) { // plusZ
      // Segment supérieur
      average = (this->faces[0][i] + this->top[i + this->scale * (this->scale-1)]) / 2;
      this->faces[0][i] = average;
      this->top[i + this->scale * (this->scale-1)] = average;

      // Segment inférieur
      average = (this->faces[0][i + this->scale * (this->scale-1)] + this->bottom[i]) / 2;
      this->faces[0][i + this->scale * (this->scale-1)] = average;
      this->bottom[i] = average;

      // Segment gauche
      average = (this->faces[0][ this->scale * i ] + this->left[i * this->scale + this->scale - 1]) / 2;
      this->faces[0][i * this->scale] = average;
      this->left[i * this->scale + this->scale - 1] = average;

      // Segment droit
      average = (this->right[ this->scale * i ] + this->faces[0][i * this->scale + this->scale - 1]) / 2;
      this->right[i * this->scale] = average;
      this->faces[0][i * this->scale + this->scale - 1] = average;
    }
    else if (faceId == 1) {
      // Segment supérieur
      average = (this->faces[1][i] + this->top[i + this->scale * (this->scale-1)]) / 2;
      this->faces[1][i] = average;
      this->top[i + this->scale * (this->scale-1)] = average;
      
      // Segment inférieur
      average = (this->faces[1][i + this->scale * (this->scale-1)] + this->bottom[i]) / 2;
      this->faces[1][i + this->scale * (this->scale-1)] = average;
      this->bottom[i] = average;

      // Segment gauche
      average = (this->faces[1][ this->scale * i ] + this->left[- i * this->scale + this->scale * (this->scale - 1)]) / 2;
      this->faces[1][ this->scale * i ] = average;
      this->left[- i * this->scale + this->scale * (this->scale - 1)] = average;

      // Segment droit
      average = (this->faces[1][ this->scale - 1 + this->scale * i ] + this->right[this->scale - 1 - i * this->scale + this->scale * (this->scale - 1)]) / 2;
      this->faces[1][this->scale - 1 + this->scale * i] = average;
      this->right[ this->scale - 1 - i * this->scale + this->scale * (this->scale-1)] = average;
    }

    else if (faceId == 2) {
      // Segment gauche
      average = (this->faces[2][ this->scale * i ] + this->left[i]) / 2;
      this->faces[2][i * this->scale] = average;
      this->left[i] = average;

      // Segment droit
      average = (this->faces[2][ this->scale - 1 + this->scale * i ] + this->right[this->scale - 1 - i]) / 2;
      this->faces[2][this->scale - 1 + i * this->scale] = average;
      this->right[ this->scale - 1 - i ] = average;
    }

    else if (faceId == 3) {
      // Segment gauche
      average = (this->faces[3][ this->scale * i ] + this->left[ this->scale * (this->scale-1) + this->scale - 1 - i]) / 2;
      this->faces[3][i * this->scale] = average;
      this->left[this->scale * (this->scale -1) + this->scale - 1 - i] = average;

      // Segment droit
      average = (this->faces[3][ this->scale - 1 + this->scale * i ] + this->right[i + this->scale * (this->scale-1)]) / 2;
      this->faces[3][this->scale - 1 + i * this->scale] = average;
      this->right[ i + this->scale * (this->scale-1) ] = average;
      
    }
  }

  // On calcule maintenant la moyenne des sommets
  // Le traitement de deux faces suffit pour faire huit fois la moyenne de trois sommets.
  // En l'occurence il faut que ces faces soit opposées

  if (faceId == 0) { // plusZ
    // Sommet supérieur gauche
    average = (this->faces[0][0] + this->top[ this->scale * (this->scale-1)] + this->left[ this->scale-1]) / 3;
    this->faces[0][0] = average;
    this->top[ this->scale * (this->scale-1)] = average;
    this->left[ this->scale-1] = average;

    // Sommet supérieur droit
    average = (this->faces[0][this->scale-1] + this->top[ (this->scale-1) + this->scale * (this->scale-1)] + this->right[0]) / 3;
    this->faces[0][this->scale-1] = average;
    this->top[ (this->scale-1) + this->scale * (this->scale-1)] = average;
    this->right[0] = average;

    // Sommet inférieur droit
    average = (this->faces[0][this->scale-1 + this->scale * (this->scale-1)] + this->bottom[ this->scale-1 ] + this->right[ this->scale * (this->scale-1)]) / 3;
    this->faces[0][ (this->scale-1) * this->scale + this->scale - 1 ] = average;
    this->bottom[ this->scale-1 ] = average;
    this->right[ this->scale * (this->scale-1) ] = average;

    // Sommet inférieur gauche
    average = (this->faces[0][this->scale * (this->scale-1)] + this->bottom[ 0 ] + this->left[ this->scale * (this->scale-1) + this->scale - 1]) / 3;
    this->faces[0][ (this->scale-1) * this->scale ] = average;
    this->bottom[0]  = average;
    this->left[ this->scale - 1 + this->scale * (this->scale-1) ] = average;
  }

  else if (faceId == 1) { // minusZ
    // Sommet supérieur gauche
    average = (this->faces[1][0] + this->top[ this->scale * (this->scale-1)] + this->left[this->scale * (this->scale-1)]) / 3;
    this->faces[1][0] = average;
    this->top[ this->scale * (this->scale-1)] = average;
    this->left[ (this->scale-1) * this->scale] = average;

    // Sommet supérieur droit
    average = (this->faces[1][this->scale-1] + this->top[ scale - 1 + scale * (scale-1)] + this->right[scale - 1 + scale * (scale-1)]) / 3;
    this->faces[1][this->scale-1] = average;
    this->top[ this->scale - 1 + this->scale * (this->scale-1)] = average;
    this->right[ this->scale - 1 + (this->scale-1) * this->scale] = average;

    // Sommet inférieur droit
    average = (this->faces[1][this->scale-1 + this->scale * (this->scale-1)] + this->bottom[this->scale-1] + this->right[this->scale-1]) / 3;
    this->faces[1][this->scale-1 + this->scale * (this->scale-1)] = average;
    this->bottom[this->scale-1] = average;
    this->right[this->scale-1] = average;

    // Sommet inférieur gauche
    average = (this->faces[1][this->scale * (this->scale-1)] + this->bottom[0] + this->left[0]) / 3;
    this->faces[1][this->scale * (this->scale-1)] = average;
    this->bottom[0] = average;
    this->left[0] = average;
  }
}
