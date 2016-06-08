#include <iostream>
#include "common.hpp"
#include "realm.hpp"
#include <cmath>

Realm::Realm(int LoD, float * min, float * max) {
  this->LoD = LoD;
  this->min = min;
  this->max= max;
  this->scale = 2 << LoD;
  this->area = 2 << (LoD+LoD+1);
  this->stampCount = 0;
  this->neighbourTop = 0;
  this->neighbourBottom = 0;
  this->neighbourLeft = 0;
  this->neighbourRight = 0;
  this->realm = new float ** [MAXIMUM_NUMBER_OF_LAYERS];
  this->realm[0] = new float*[1]();

  for (int i = 0; i < MAXIMUM_NUMBER_OF_LAYERS-1; i++) {
    this->realm[i+1] = new float*[ 2 << (i * 2 + 1) ]();
  }

  this->AllocateChunk(0,0,0); 
}

inline void Realm::UpdateMinMax(int chunkIndex, float * chunk) {
  if (*this->max < chunk[ chunkIndex ]) {
    *this->max = chunk[ chunkIndex ];
  }
  if (*this->min <= 0 && *this->min > chunk[ chunkIndex ]) {
    *this->min = chunk[ chunkIndex ];
  }
}

void Realm::SetNeighbours(
      float *** neighbourTop,
      float *** neighbourBottom,
      float *** neighbourLeft,
      float *** neighbourRight
  ) {
  this->neighbourTop = neighbourTop;
  this->neighbourBottom = neighbourBottom;
  this->neighbourLeft = neighbourLeft;
  this->neighbourRight = neighbourRight;
};

void Realm::AddStamp(float * stamp) {
  this->stamps[this->stampCount]  = stamp;
  this->stampCount++;
}

void Realm::AllocateChunk(int layer, int chunkCoordX, int chunkCoordY) {
  this->realm[layer][chunkCoordX + chunkCoordY * (1 << layer)] = new float[this->area]; 
}

void Realm::DeallocateChunk(int layer, int chunkCoordX, int chunkCoordY) {
  delete [] this->realm[layer][chunkCoordX + chunkCoordY * (0 << layer)];
}

float * Realm::GetRealm(int layer, int chunkCoordX, int chunkCoordY) {
  return this->realm[layer][chunkCoordX + chunkCoordY * (0 << layer)];
}

float *** Realm::GetRealm() {
  return this->realm;
}

void Realm::Noise(int layer, int chunkCoordX, int chunkCoordY) {
  this->Noise(layer, chunkCoordX, chunkCoordY, this->scale, 0, 0);
}

inline bool Realm::IsChunkAllocated(float *** dest, int layer, int chunkIndex) {
  if(dest != 0) {
    if (dest[layer] != 0) {
      if(dest[layer][ chunkIndex ] != 0) {
        return true;
      }
    }
  }
  return false;
}


inline bool Realm::DoesStampCrossCorner(int offsetX, int offsetY, int sectorScale) {
  int stampRadius = sectorScale >> 1;
  int xCornerTopLeft = offsetX+stampRadius;
  int yCornerTopLeft = offsetY+stampRadius;
  int xCornerTopRight = stampRadius - (offsetX + sectorScale - this->scale);
  int yCornerTopRight = offsetY+stampRadius;
  int xCornerBottomRight = stampRadius - (offsetX + sectorScale - this->scale);
  int yCornerBottomRight = this->scale - (offsetY + stampRadius);
  int xCornerBottomLeft = offsetX+stampRadius;
  int yCornerBottomLeft = this->scale - (offsetY + stampRadius);

  // Coin gauche supérieur
  if(offsetX < 0 && offsetY < 0 && ceil(sqrt(pow(xCornerTopLeft,2) + pow(yCornerTopLeft,2))) <= stampRadius) {
      return true;
  }
  // Coin supérieur droit
  else if (offsetX+sectorScale >= this->scale && offsetY < 0 && ceil(sqrt(pow(xCornerTopRight,2) + pow(yCornerTopRight,2))) <= stampRadius) {
    return true;
  }
  // Coin inférieur droit
  else if (offsetX+sectorScale >= this->scale && offsetY+sectorScale >= this->scale && ceil(sqrt(pow(xCornerBottomRight,2) + pow(yCornerBottomRight,2))) <= stampRadius) {
    return true;
  }
  // Coin inférieur gauche
  else if (offsetX < 0 && offsetY+sectorScale >= this->scale && ceil(sqrt(pow(xCornerBottomLeft,2) + pow(yCornerBottomLeft,2))) <= stampRadius) {
    return true;
  }
  return false;
}

void Realm::Noise(int layer, int chunkCoordX, int chunkCoordY, int sectorScale, int sectorStartU, int sectorStartV) {
  if (sectorScale == 1) {
    return;
  }


  float sign = getRandom() & 1 ? 1.0 : -1.0;

  int halfScale = sectorScale >> 1;
  int randX = - halfScale + getRandom() % (sectorScale);
  int randY = - halfScale + getRandom() % (sectorScale);
  int stampId =  getRandom() % this->stampCount;
  int inc = this->scale / sectorScale;
  int stampIndex;
  int chunkIndex;
  int offsetX = randX + sectorStartU;
  int offsetY = randY + sectorStartV;
  int stampX=0,stampY=0;
  double distanceFromStampCenterToCorner;

  // Cette curieuse formule permet de connaitre pour un calque donné la dimension
  // d'un morceau courant. Habile!
  int chunkScale = 1 << layer;
  int localChunkCoord = chunkCoordX + chunkCoordY * chunkScale;

  float * localChunk= this->realm[layer][localChunkCoord];
  float * stamp = this->stamps[stampId];

  // Selon le cas de figure, le dépassement du tampon peut déborder sur au plus trois chunk:
  // Pour éviter de déréférencer 10 milles fois, on prépare les destinations en fonctions des paramétres
  // de la récursion courante.
  //
  //
  //
  //
  //
    int horizontalNeighbourChunkCoord = 0;
    int verticalNeighbourChunkCoord = 0;
    float * horizontalNeighbourChunk = 0;
    float * verticalNeighbourChunk = 0;
    float * diagonalNeighbourChunk = 0;

    // Dépassement sur la gauche
    if (offsetX < 0) {
      if (chunkCoordX == 0) {
        horizontalNeighbourChunkCoord = this->getCoordsToNeighbourLeft(-1, chunkCoordY, chunkScale);
        if (this->IsChunkAllocated(this->neighbourLeft, layer, horizontalNeighbourChunkCoord) == true) {
          horizontalNeighbourChunk = this->neighbourLeft[layer][horizontalNeighbourChunkCoord ]; 
        }
      }
      else {
        horizontalNeighbourChunkCoord = chunkCoordX - 1 + chunkCoordY * chunkScale;
        horizontalNeighbourChunk = this->realm[layer][horizontalNeighbourChunkCoord];
      }
    }
    //Dépassement sur la droite
    else if (offsetX+sectorScale >= this->scale) {
      if (chunkCoordX == chunkScale - 1) {
        horizontalNeighbourChunkCoord = this->getCoordsToNeighbourRight(chunkCoordX+1, chunkCoordY, chunkScale);
        if (this->IsChunkAllocated(this->neighbourRight, layer, horizontalNeighbourChunkCoord) == true) {
          horizontalNeighbourChunk = this->neighbourRight[layer][horizontalNeighbourChunkCoord ]; 
        }
      }
      else {
        horizontalNeighbourChunkCoord = chunkCoordX + 1 + chunkCoordY * chunkScale;
        horizontalNeighbourChunk = this->realm[layer][horizontalNeighbourChunkCoord];
      }
    }
    //Dépassement vers le bas
    if (offsetY+sectorScale >= this->scale) {
      if (chunkCoordY == chunkScale - 1) {
        verticalNeighbourChunkCoord = this->getCoordsToNeighbourBottom(chunkCoordX, chunkCoordY+1, chunkScale);
        if (this->IsChunkAllocated(this->neighbourBottom, layer, verticalNeighbourChunkCoord) == true) {
          verticalNeighbourChunk = this->neighbourBottom[layer][verticalNeighbourChunkCoord ]; 
        }
      }
      else {
        verticalNeighbourChunkCoord = chunkCoordX + (chunkCoordY+1) * chunkScale;
        verticalNeighbourChunk = this->realm[layer][verticalNeighbourChunkCoord];
      }
    }
    //Dépassement vers le haut
    else if (offsetY < 0 ) {
      if (chunkCoordY == 0) {
        verticalNeighbourChunkCoord = this->getCoordsToNeighbourTop(chunkCoordX, -1, chunkScale);
        if (this->IsChunkAllocated(this->neighbourTop, layer, verticalNeighbourChunkCoord) == true) {
          verticalNeighbourChunk = this->neighbourTop[layer][ verticalNeighbourChunkCoord ]; 
        }
      }
      else {
        verticalNeighbourChunkCoord = chunkCoordX + (chunkCoordY-1) * chunkScale;
        verticalNeighbourChunk = this->realm[layer][verticalNeighbourChunkCoord];
      }
    }
                  
                                                                                                  //
                                                                                                  //
                                                                                                  //
                                                                                                  //
  // Fin du bloc de préparation des destinations. //////////////////////////////////////////////////
  // Arrivé ici, les références des destinations sont préparées quelques soit le cas de figure.

  // Premier cas: Quand le tampon est exactement contenu à l'intérieur d'une face
  if ( offsetY >= 0 && offsetY+sectorScale-1 < this->scale && offsetX >= 0 && offsetX+sectorScale-1< this->scale) {
    localChunk = this->realm[layer][chunkCoordX + chunkCoordY * chunkScale];
    for (int y=0; y<sectorScale; y++) {
      stampX = 0;
      for (int x=0; x<sectorScale; x++) {
        stampIndex = stampX + sectorScale * stampY;
	if ( stamp[ stampIndex ] != 0) {
	  chunkIndex = (y+offsetY) * this->scale + x+offsetX;
	  localChunk[ chunkIndex ] += sign * stamp[ stampIndex ] / inc;
          this->UpdateMinMax(chunkIndex, localChunk);
	}
	stampX += inc;
      }
      stampY += inc * inc;
    }
  }

  // Second cas: Situation relativement simple de dépassement en X et/ou Y sans que le tampon croise un coin.
  // On distingue alors deux cas, quand le pixel dépasse du secteur, en dehor du royaume courant, ou quand
  // il dépasse du secteur courant à l'intérieur du royaume.
  else if ( !this->DoesStampCrossCorner(offsetX,offsetY,sectorScale) ) { 
    for (int y=0; y<sectorScale; y++) {
      stampX = 0;
      for (int x=0; x<sectorScale; x++) {
        stampIndex = stampX + sectorScale * stampY;
	if ( stamp[ stampIndex ] != 0) {
          // Le pixel courant est à gauche du secteur courant.
          if(horizontalNeighbourChunk != 0 && offsetX+x < 0) {
            // Le pixel courant se trouve sur la gauche du secteur courant, mais à l'extérieur du royaume.
            if (chunkCoordX == 0) {
	        chunkIndex = this->getCoordsToNeighbourLeft( offsetX+x, y+offsetY, this->scale) ;
            }
            // Le pixel courant se trouve sur la gauche du secteur courant, mais à l'intérieur du royaume.
            else {
	      chunkIndex = (y+offsetY) * this->scale + this->scale - x+offsetX;
            }
            horizontalNeighbourChunk[chunkIndex] += sign * stamp[ stampIndex ] / inc;
            this->UpdateMinMax(chunkIndex, horizontalNeighbourChunk);
          }
          // Le pixel courant est à droite du secteur courant.
          else if (horizontalNeighbourChunk != 0 && offsetX+x >= this->scale) {
            // Le pixel courant se trouve sur la droite du secteur courant, mais à l'extérieur du royaume.
            if (chunkCoordX == chunkScale - 1) {
	        chunkIndex = this->getCoordsToNeighbourRight( offsetX+x, y+offsetY, this->scale) ;
            }
            // Le pixel courant se trouve sur la droite du secteur courant, mais à l'intérieur du royaume.
            else {
	      chunkIndex = (y+offsetY) * this->scale + x+offsetX - this->scale;
            }
            horizontalNeighbourChunk[chunkIndex] += sign * stamp[ stampIndex ] / inc;
            this->UpdateMinMax(chunkIndex, horizontalNeighbourChunk);
          }
          // Le pixel courant est en bas du secteur courant
          else if (verticalNeighbourChunk != 0 && offsetY+y >= this->scale) {
            // Le pixel courant se trouve en bas du secteur courant, mais à l'extérieur du royaume.
            if (chunkCoordY == chunkScale - 1) {
	        chunkIndex = this->getCoordsToNeighbourBottom( offsetX+x, offsetY+y, this->scale) ;
            }
            // Le pixel courant se trouve en bas du secteur courant, mais à l'intérieur du royaume.
            else {
	      chunkIndex = (y+offsetY) * this->scale + x+offsetX - this->scale;
            }
            verticalNeighbourChunk[ chunkIndex ] += sign * stamp[ stampIndex ] / inc;
            this->UpdateMinMax(chunkIndex, verticalNeighbourChunk);
          }
          // Le pixel courant est en haut du secteur courant
          else if (verticalNeighbourChunk != 0 && offsetY+y < 0) {
            // Le pixel courant se trouve en haut du secteur courant, mais à l'extérieur du royaume.
            if (chunkCoordY == 0) {
	      chunkIndex = this->getCoordsToNeighbourTop( offsetX+x, offsetY+y, this->scale) ;
            }
            // Le pixel courant se trouve en bas du secteur courant, mais à l'intérieur du royaume.
            else {
	      chunkIndex = (this->scale - y+offsetY) * this->scale + x+offsetX;
            }
            verticalNeighbourChunk[ chunkIndex ] += sign * stamp[ stampIndex ] / inc;
            this->UpdateMinMax(chunkIndex, verticalNeighbourChunk);
          }
          // Le pixel courant est à l'intérieur du secteur courant
          else if (x+offsetX >= 0 && x+offsetX < this->scale && y+offsetY >= 0 && y+offsetY < this->scale) {
            chunkIndex = (y+offsetY) * this->scale + x+offsetX;
	    localChunk[ chunkIndex ] += sign * stamp[ stampIndex ] / inc;
            this->UpdateMinMax(chunkIndex, localChunk);
          }
        }
	stampX += inc;
      }
      stampY += inc * inc;
    }
  }

  this->Noise(layer, chunkCoordX, chunkCoordY, halfScale, sectorStartU,		  sectorStartV);
  this->Noise(layer, chunkCoordX, chunkCoordY, halfScale, sectorStartU, 	  sectorStartV+halfScale);
  this->Noise(layer, chunkCoordX, chunkCoordY, halfScale, sectorStartU+halfScale, sectorStartV+halfScale);
  this->Noise(layer, chunkCoordX, chunkCoordY, halfScale, sectorStartU+halfScale, sectorStartV);
};
