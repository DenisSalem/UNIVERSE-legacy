#include <iostream>
#include <cmath>
#include "../common.hpp"
#include "cubeMap.hpp"

CubeMap::CubeMap(int LOD) {
  this->heightMapScale        = (2 << (LOD + 1));
  this->displacementMapScale  = (2 << (LOD - 1));
  int heightMapArea           = this->heightMapScale*this->heightMapScale;
  int displacementMapArea     = this->displacementMapScale*this->displacementMapScale;
  
  this->stamp = new float[heightMapArea];

  std::cout << "Initiate six heightmaps of " << this->heightMapScale << " pixels²...\n";

  // On initialise les six heightmaps du cube.
  for (int i = 0; i<6; i++) {
    this->heightMaps[i]	= new float[heightMapArea];
  }

  std::cout << "Initiate six displacement map  of " << this->displacementMapScale << " pixels²...\n";
  // On initialise les six displacement map du cube.
  for (int i = 0; i<6; i++) {
    this->displacementMaps[i]	= new float[displacementMapArea];
  }

  // On fait en sorte que chaque heightmap soient vierge
  for(int y = 0; y<heightMapArea; y+= this->heightMapScale) {
    for(int x = 0; x<this->heightMapScale; x++) {
      for (int i = 0; i<6; i++) {
        this->heightMaps[i][x + y] = 0;
      }
    }
  }
  
  // On crée un tampon
  this->Stamp();

  // Pour chaque face qu'on va traiter, il faut déterminer les faces voisines.

  this->top    = this->heightMaps[2];
  this->bottom = this->heightMaps[3];
  this->left   = this->heightMaps[5];
  this->right  = this->heightMaps[4];
  this->Noise(this->heightMapScale, 0, 0, 0);

  this->top    = this->heightMaps[3];
  this->bottom = this->heightMaps[2];
  this->left   = this->heightMaps[5];
  this->right  = this->heightMaps[4];
  this->Noise(this->heightMapScale, 0, 0, 1);

  this->top     = this->heightMaps[1];
  this->bottom  = this->heightMaps[0];
  this->left    = this->heightMaps[5];
  this->right   = this->heightMaps[4];
  this->Noise(this->heightMapScale, 0, 0, 2);

  this->top     = this->heightMaps[0];
  this->bottom  = this->heightMaps[1];
  this->left    = this->heightMaps[5];
  this->right   = this->heightMaps[4];

  this->Noise(this->heightMapScale, 0, 0, 3);

  this->top     = this->heightMaps[2];
  this->bottom  = this->heightMaps[3];
  this->left    = this->heightMaps[0];
  this->right   = this->heightMaps[1];
  this->Noise(this->heightMapScale, 0, 0, 4);

  this->top     = this->heightMaps[2];
  this->bottom  = this->heightMaps[3];
  this->left    = this->heightMaps[1];
  this->right   = this->heightMaps[0];
  this->Noise(this->heightMapScale, 0, 0, 5);


  //On copie les heightmaps dans les displacement maps qui elles sont réduites et à l'échelle des grilles de vertex.
  for (int x = 0; x < this->displacementMapScale; x++) { 
    for (int y = 0; y < this->displacementMapScale; y++) {
      for(int i = 0; i < 6; i++) {
        this->displacementMaps[i][ this->displacementMapScale * y + x] = this->heightMaps[i][this->heightMapScale * y * 4 + x * 4];
      }
    }
  }
  // Cela étant fait il faut racorder chacune des faces. En effet, en l'état, 
  // si les heighmaps sont appliqué à la sphére projeté il y aura des défauts
  // de jointures entre chacunes des faces du cube.

  this->top    = this->displacementMaps[2];
  this->bottom = this->displacementMaps[3];
  this->left   = this->displacementMaps[5];
  this->right  = this->displacementMaps[4];
  this->MergeEdges(0);

  this->top    = this->displacementMaps[3];
  this->bottom = this->displacementMaps[2];
  this->left   = this->displacementMaps[5];
  this->right  = this->displacementMaps[4];
  this->MergeEdges(1);

  // Maintenant que nous avons calculé deux faces opposées, tous les sommets sont bon.
  // Huits segments sur douze sont également calculés.

  this->top     = this->displacementMaps[1];
  this->bottom  = this->displacementMaps[0];
  this->left    = this->displacementMaps[5];
  this->right   = this->displacementMaps[4];
  this->MergeEdges(2);

  this->top     = this->displacementMaps[0];
  this->bottom  = this->displacementMaps[1];
  this->left    = this->displacementMaps[5];
  this->right   = this->displacementMaps[4];
  this->MergeEdges(3);

  GLubyte * tmpArray;

  for (int i = 0; i < 6; i++) {

    glGenTextures(1, &this->textureID[i]);
    glBindTexture(GL_TEXTURE_2D, this->textureID[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      tmpArray = new GLubyte[ heightMapArea ];

      for(int x = 0; x < this->heightMapScale; x++) {
        for(int y = 0; y < this->heightMapScale; y++) {
          if(i==0) {
            tmpArray[x + y * this->heightMapScale] = (255 * (this->heightMaps[i][this->heightMapScale * (this->heightMapScale - 1 - y) + x ] - this->min) / (this->max - this->min));
          }
          else if (i==1) {
            tmpArray[x + y * this->heightMapScale] = (255 * (this->heightMaps[i][this->heightMapScale * y + x ] - this->min) / (this->max - this->min));
          }
          else if (i==2) {
            tmpArray[x + y * this->heightMapScale] = (255 * (this->heightMaps[i][this->heightMapScale * y + x ] - this->min) / (this->max - this->min));
          }
          else if (i==3) {
            tmpArray[x + y * this->heightMapScale] = (255 * (this->heightMaps[i][this->heightMapScale * (this->heightMapScale - 1 - y) + x ] - this->min) / (this->max - this->min));
          }
          else if (i==4) {
            tmpArray[x + y * this->heightMapScale] = (255 * (this->heightMaps[i][this->heightMapScale * (this->heightMapScale - 1 - x) + (this->heightMapScale - 1 - y) ] - this->min) / (this->max - this->min));
          }
          else if (i==5) {
            tmpArray[x + y * this->heightMapScale] = (255 * (this->heightMaps[i][this->heightMapScale * (this->heightMapScale - 1 - x) + y ] - this->min) / (this->max - this->min));
          }
        }
      }

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->heightMapScale, this->heightMapScale, 0, GL_RED, GL_UNSIGNED_BYTE, tmpArray);
      glGenerateMipmap(GL_TEXTURE_2D); 

      delete [] tmpArray;
    glBindTexture(GL_TEXTURE_2D, 0);
  }
};

void CubeMap::Stamp() {
  int x,y,X=0;
  double halfScale = this->heightMapScale / 2 ;
  double radius;

  //int * powersOfTwo = (int *) malloc( sizeof(int) * this->scale);
  int * powersOfTwo = new int[this->heightMapScale];
  // On crée deux tables contenants les valeurs élevées à la puissance de deux.
  // On calcul ainsi n fois ces valeurs au lieu de n².
  for(x=0; x<this->heightMapScale;x++) {
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
  int limit = this->heightMapScale*this->heightMapScale;
  for(x=0;x<limit;x+=this->heightMapScale) {
    for(y=0;y<this->heightMapScale;y++) {
      // On calcule le rayon du cercle sur lequel se trouve le point courant.
      // Opération très TRÈS gourmante en temps CPU
      radius = sqrtl( powersOfTwo[y] + powersOfTwo[X]);
      if ( radius < halfScale ) {
        // y a plus qu'à dessiner le cône.
	this->stamp[x+y] = (halfScale - radius) / (halfScale);
      }
      else {
        // Si on est en dehors du cercle, on se casse pas la tête et on affecte un zero.
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
  int inc = this->heightMapScale / scale;
  int stampIndex;
  int faceIndex;
  int stampX=0,stampY=0;
	

  float * dest;

  // Quand le tampon est contenu exactement à l'intérieur d'une face
  if ((randY+offsetY >= 0 && randY+offsetY+scale-1 < this->heightMapScale) && (randX+offsetX >= 0 && randX+offsetX+scale-1< this->heightMapScale)) {
    for(int y=0; y<scale;y++) {
      stampX = 0;
      for(int x=0; x<scale;x++) {
        stampIndex = stampX + scale * stampY;
	if( this->stamp[stampIndex] != 0) {
	  faceIndex = (y+randY+offsetY) * this->heightMapScale + x+randX+offsetX;
	  this->heightMaps[faceId][faceIndex] += sign * this->stamp[stampIndex] / inc;
	  if (this->max < this->heightMaps[faceId][ faceIndex]) {
	    this->max = this->heightMaps[faceId][ faceIndex ];
	  }
	  if (this->min <= 0 && this->min > this->heightMaps[faceId][ faceIndex ]) {
	    this->min = this->heightMaps[faceId][ faceIndex ];
	  }
	}
	stampX += inc;
      }
      stampY += (inc * inc);
    }
  } 
  // Quand le tampon dépasse sur l'axe X
  else if ( !(randX+offsetX >= 0 && randX+offsetX+scale-1 < this->heightMapScale) && ( randY+offsetY >= 0 && randY+offsetY+scale-1 < this->heightMapScale )) {
    for(int y=0; y<scale;y++) {
      stampX = 0;
      for(int x=0; x<scale;x++) {
        stampIndex = stampX + scale * stampY;
	if( this->stamp[stampIndex] != 0) {
	  if (randX+offsetX+x < 0) {
	    dest = this->left;
	    if (faceId == 1) {
	      faceIndex = ( this->heightMapScale - (y+randY+offsetY) ) * this->heightMapScale - (x+randX+offsetX)-1;
	    }
	    else if (faceId == 2) {
	      faceIndex = -(x+randX+offsetX + 1) * this->heightMapScale +(y+randY+offsetY);
	    }
	    else if (faceId == 3) {
	      faceIndex = ( this->heightMapScale + x+randX+offsetX - 1) * this->heightMapScale + this->heightMapScale - 1 - (y+randY+offsetY);
	    }
	    else if (faceId == 4) {
	      faceIndex = (y+randY+offsetY) * this->heightMapScale + this->heightMapScale + (x+randX+offsetX);
	    }
	    else if (faceId == 5) {
	      faceIndex = ( this->heightMapScale - (y+randY+offsetY)) * this->heightMapScale - (x+randX+offsetX) - 1;
	    }
	    else {
	      faceIndex = (y+randY+offsetY) * this->heightMapScale + this->heightMapScale + (x+randX+offsetX);
	    }
	  }
	  else if ( randX+offsetX+x >= this->heightMapScale ) {
	    dest = this->right;
	    if (faceId == 1) {
	      faceIndex = (this->heightMapScale - (y+randY+offsetY)) * this->heightMapScale + this->heightMapScale - ((x+randX+offsetX) - this->heightMapScale) - 1;
	    }
	    else if (faceId == 2) {
	      faceIndex = ((x+randX+offsetX) - this->heightMapScale) * this->heightMapScale + this->heightMapScale - (y+randY+offsetY);
	    }
	    else if (faceId == 3) {
	      faceIndex = ( this->heightMapScale - ((x+randX+offsetX) - this->heightMapScale) - 1) * this->heightMapScale + (y+randY+offsetY);
	    }
	    else if (faceId == 4) {
	      faceIndex = ( this->heightMapScale - (y+randY+offsetY) ) * this->heightMapScale + this->heightMapScale - ((x+randX+offsetX) - this->heightMapScale) - 1;
	    }
	    else if (faceId == 5) {
	      faceIndex = (y+randY+offsetY) * this->heightMapScale + (x+randX+offsetX) - this->heightMapScale;
            }
	    else {
	      faceIndex = (y+randY+offsetY) * this->heightMapScale + x+randX+offsetX - this->heightMapScale;
	    }
	  }
	  else {
	    dest = this->heightMaps[faceId];
	    faceIndex = ((y+randY+offsetY)) * this->heightMapScale + x+randX+offsetX;
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
  else if ((randX+offsetX >= 0 && randX+offsetX+scale-1 <this->heightMapScale) && !( randY+offsetY >= 0 && randY+offsetY+scale-1 < this->heightMapScale  )) {
    for(int y=0; y<scale;y++) {
      stampX = 0;
      for(int x=0; x<scale;x++) {
        stampIndex = stampX + scale * stampY;
	if( this->stamp[stampIndex] != 0) {
	  if (randY+offsetY+y < 0) {
	    dest = this->top;
	    if ( faceId == 4) {
	      faceIndex = ( this->heightMapScale + (y+randY+offsetY)) + this->heightMapScale * (this->heightMapScale - (x+randX+offsetX));
	    }
	    else if ( faceId == 5) {
	      faceIndex = (x+randX+offsetX) * this->heightMapScale - 1 - y-randY-offsetY;
	    }
	    else {
	      faceIndex = (this->heightMapScale + (y+randY+offsetY)) * this->heightMapScale + x+randX+offsetX;
	    }
	  }
	  else if (randY+offsetY+y >= this->heightMapScale) {
	    dest = this->bottom;
	    if ( faceId == 4 ) {
	      faceIndex = - (y+randY+offsetY) - 1 + (this->heightMapScale * ( scale - x+randX+offsetX) );
	    }
	    else if(faceId == 5) {
	      faceIndex = (y+randY+offsetY - this->heightMapScale) + this->heightMapScale * (this->heightMapScale -x-randX-offsetX);
	    }
	    else {
	      faceIndex = (y+randY+offsetY - this->heightMapScale) * this->heightMapScale + x+randX+offsetX;
	    }
	  }
	  else {
	    dest = this->heightMaps[faceId];
	    faceIndex = ((y+randY+offsetY)) * this->heightMapScale + x+randX+offsetX;
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
  // On parcours les quatres segments composants la face courante en une seule itération
  // On ne touche pas aux sommet du carré pour le moment, ils seront traités séparément.
  // En fait les sommets de la face courrante seront le résultat de la moyenne des sommets voisins
  // en un point donné.
  
  float average;
  for(int i = 1; i <this->displacementMapScale - 1; i++ ) {
    if(faceId == 0) { // plusZ
      // Segment supérieur
      average = (this->displacementMaps[0][i] + this->top[i + this->displacementMapScale * (this->displacementMapScale-1)]) / 2;
      this->displacementMaps[0][i] = average;
      this->top[i + this->displacementMapScale * (this->displacementMapScale-1)] = average;

      // Segment inférieur
      average = (this->displacementMaps[0][i + this->displacementMapScale * (this->displacementMapScale-1)] + this->bottom[i]) / 2;
      this->displacementMaps[0][i + this->displacementMapScale * (this->displacementMapScale-1)] = average;
      this->bottom[i] = average;

      // Segment gauche
      average = (this->displacementMaps[0][ this->displacementMapScale * i ] + this->left[i * this->displacementMapScale + this->displacementMapScale - 1]) / 2;
      this->displacementMaps[0][i * this->displacementMapScale] = average;
      this->left[i * this->displacementMapScale + this->displacementMapScale - 1] = average;

      // Segment droit
      average = (this->right[ this->displacementMapScale * i ] + this->displacementMaps[0][i * this->displacementMapScale + this->displacementMapScale - 1]) / 2;
      this->right[i *this->displacementMapScale] = average;
      this->displacementMaps[0][i * this->displacementMapScale + this->displacementMapScale - 1] = average;
    }
    else if (faceId == 1) {
      // Segment supérieur
      average = (this->displacementMaps[1][i] + this->top[i + this->displacementMapScale * (this->displacementMapScale-1)]) / 2;
     this->displacementMaps[1][i] = average;
      this->top[i + this->displacementMapScale * (this->displacementMapScale-1)] = average;
      
      // Segment inférieur
      average = (this->displacementMaps[1][i + this->displacementMapScale * (this->displacementMapScale-1)] + this->bottom[i]) / 2;
      this->displacementMaps[1][i + this->displacementMapScale * (this->displacementMapScale-1)] = average;
      this->bottom[i] = average;

      // Segment gauche
      average = (this->displacementMaps[1][ this->displacementMapScale * i ] + this->left[- i * this->displacementMapScale + this->displacementMapScale * (this->displacementMapScale - 1)]) / 2;
      this->displacementMaps[1][ this->displacementMapScale * i ] = average;
      this->left[- i * this->displacementMapScale + this->displacementMapScale * (this->displacementMapScale - 1)] = average;

      // Segment droit
      average = (this->displacementMaps[1][ this->displacementMapScale - 1 + this->displacementMapScale * i ] + this->right[this->displacementMapScale - 1 - i * this->displacementMapScale + this->displacementMapScale * (this->displacementMapScale - 1)]) / 2;
     this->displacementMaps[1][this->displacementMapScale - 1 + this->displacementMapScale * i] = average;
      this->right[ this->displacementMapScale - 1 - i * this->displacementMapScale + this->displacementMapScale * (this->displacementMapScale-1)] = average;
    }

    else if (faceId == 2) {
      // Segment gauche
      average = (this->displacementMaps[2][ this->displacementMapScale * i ] + this->left[i]) / 2;
      this->displacementMaps[2][i * this->displacementMapScale] = average;
      this->left[i] = average;

      // Segment droit
      average = (this->displacementMaps[2][ this->displacementMapScale - 1 + this->displacementMapScale * i ] + this->right[this->displacementMapScale - 1 - i]) / 2;
      this->displacementMaps[2][this->displacementMapScale - 1 + i * this->displacementMapScale] = average;
      this->right[ this->displacementMapScale - 1 - i ] = average;
    }

    else if (faceId == 3) {
      // Segment gauche
      average = (this->displacementMaps[3][ this->displacementMapScale * i ] + this->left[ this->displacementMapScale * (this->displacementMapScale-1) + this->displacementMapScale - 1 - i]) / 2;
      this->displacementMaps[3][i * this->displacementMapScale] = average;
      this->left[this->displacementMapScale * (this->displacementMapScale -1) + this->displacementMapScale - 1 - i] = average;

      // Segment droit
      average = (this->displacementMaps[3][ this->displacementMapScale - 1 + this->displacementMapScale * i ] + this->right[i + this->displacementMapScale * (this->displacementMapScale-1)]) / 2;
      this->displacementMaps[3][this->displacementMapScale - 1 + i * this->displacementMapScale] = average;
      this->right[ i + this->displacementMapScale * (this->displacementMapScale-1) ] = average;
      
    }
  }

  // On calcul maintenant la moyenne des sommets
  // Le traitement de deux faces suffit pour faire huit fois la moyenne de trois sommets.
  // En l'occurence il faut que ces faces soit opposé

  if (faceId == 0) { // plusZ
    // Sommet supérieur gauche
    average = (this->displacementMaps[0][0] + this->top[ this->displacementMapScale * (this->displacementMapScale-1)] + this->left[ this->displacementMapScale-1]) / 3;
    this->displacementMaps[0][0] = average;
    this->top[ this->displacementMapScale * (this->displacementMapScale-1)] = average;
    this->left[ this->displacementMapScale-1] = average;

    // Sommet supérieur droit
    average = (this->displacementMaps[0][this->displacementMapScale-1] + this->top[ (this->displacementMapScale-1) + this->displacementMapScale * (this->displacementMapScale-1)] + this->right[0]) / 3;
    this->displacementMaps[0][this->displacementMapScale-1] = average;
    this->top[ (this->displacementMapScale-1) + this->displacementMapScale * (this->displacementMapScale-1)] = average;
    this->right[0] = average;

    // Sommet inférieur droit
    average = (this->displacementMaps[0][this->displacementMapScale-1 + this->displacementMapScale * (this->displacementMapScale-1)] + this->bottom[ this->displacementMapScale-1 ] + this->right[ this->displacementMapScale * (this->displacementMapScale-1)]) / 3;
    this->displacementMaps[0][ (this->displacementMapScale-1) * this->displacementMapScale + this->displacementMapScale - 1 ] = average;
    this->bottom[ this->displacementMapScale-1 ] = average;
    this->right[ this->displacementMapScale * (this->displacementMapScale-1) ] = average;

    // Sommet inférieur gauche
    average = (this->displacementMaps[0][this->displacementMapScale * (this->displacementMapScale-1)] + this->bottom[ 0 ] + this->left[ this->displacementMapScale * (this->displacementMapScale-1) + this->displacementMapScale - 1]) / 3;
    this->displacementMaps[0][ (this->displacementMapScale-1) * this->displacementMapScale ] = average;
    this->bottom[0]  = average;
    this->left[ this->displacementMapScale - 1 + this->displacementMapScale * (this->displacementMapScale-1) ] = average;
  }

  else if (faceId == 1) { // minusZ
    // Sommet supérieur gauche
    average = (this->displacementMaps[1][0] + this->top[ this->displacementMapScale * (this->displacementMapScale-1)] + this->left[this->displacementMapScale * (this->displacementMapScale-1)]) / 3;
    this->displacementMaps[1][0] = average;
    this->top[ this->displacementMapScale * (this->displacementMapScale-1)] = average;
    this->left[ (this->displacementMapScale-1) * this->displacementMapScale] = average;

    // Sommet supérieur droit
    average = (this->displacementMaps[1][this->displacementMapScale-1] + this->top[ this->displacementMapScale - 1 + this->displacementMapScale * (this->displacementMapScale-1)] + this->right[this->displacementMapScale - 1 + this->displacementMapScale * (this->displacementMapScale-1)]) / 3;
    this->displacementMaps[1][this->displacementMapScale-1] = average;
    this->top[ this->displacementMapScale - 1 + this->displacementMapScale * (this->displacementMapScale-1)] = average;
    this->right[ this->displacementMapScale - 1 + (this->displacementMapScale-1) * this->displacementMapScale] = average;

    // Sommet inférieur droit
    average = (this->displacementMaps[1][this->displacementMapScale-1 + this->displacementMapScale * (this->displacementMapScale-1)] + this->bottom[this->displacementMapScale-1] + this->right[this->displacementMapScale-1]) / 3;
    this->displacementMaps[1][this->displacementMapScale-1 + this->displacementMapScale * (this->displacementMapScale-1)] = average;
    this->bottom[this->displacementMapScale-1] = average;
    this->right[this->displacementMapScale-1] = average;

    // Sommet inférieur gauche
    average = (this->displacementMaps[1][this->displacementMapScale * (this->displacementMapScale-1)] + this->bottom[0] + this->left[0]) / 3;
   this->displacementMaps[1][this->displacementMapScale * (this->displacementMapScale-1)] = average;
    this->bottom[0] = average;
    this->left[0] = average;
  }
}
