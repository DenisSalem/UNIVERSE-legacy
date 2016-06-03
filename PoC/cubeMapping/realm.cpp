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
  this->neighbourTop;
  this->neighbourBottom;
  this->neighbourLeft;
  this->neighbourRight;
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

inline bool Realm::IsChunkAllocated(int layer, int chunkIndex) {
  if (this->neighbourLeft[layer] != 0) {
    if(this->neighbourLeft[layer][ chunkIndex ] != 0) {
      return true;
    }
  }
  return false;
}


inline bool Realm::DoesStampCrossCorner(int offsetX, int offsetY, int sectorScale) {
  int stampRadius = sectorScale >> 1;
  int sectorScaleMinusScale = sectorScale - this->scale;
  int xCornerTopLeft = offsetX+stampRadius;
  int yCornerTopLeft = offsetY+stampRadius;
  int xCornerTopRight = offsetX + sectorScaleMinusScale;
  int yCornerTopRight = offsetY;
  int xCornerBottomRight = offsetX + sectorScaleMinusScale;
  int yCornerBottomRight = offsetY + sectorScaleMinusScale;
  int xCornerBottomLeft = offsetX+stampRadius;
  int yCornerBottomLeft = this->scale - (offsetY + stampRadius);

  // Coin gauche supérieur
  if(offsetX < 0 && offsetY < 0 && ceil(sqrt(pow(xCornerTopLeft,2) + pow(yCornerTopLeft,2))) <= stampRadius) {
      return true;
  }
  else if (offsetX+sectorScale >= this->scale && offsetY < 0 && ceil(sqrt(pow(xCornerTopRight,2) + pow(yCornerTopRight,2))) <= stampRadius) {
    return true;
  }
  /*else if (offsetX+sectorScale >= this->scale && offsetY+sectorScale >= this->scale) {
    if( ceil(sqrt(pow(xCornerBottomRight,2) + pow(yCornerBottomRight,2))) <= stampRadius) {
      return true;
    }
    else {
      return true;
    }
  }*/

  else if (offsetX < 0 && offsetY+sectorScale >= this->scale && ceil(sqrt(pow(xCornerBottomLeft,2) + pow(yCornerBottomLeft,2))) <= stampRadius) {
    return true;
  }
  return false;
}

void Realm::Noise(int layer, int chunkCoordX, int chunkCoordY, int sectorScale, int sectorStartU, int sectorStartV) {
  if (sectorScale == 256) {
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
  int offsetY = randY + sectorStartV;
  int offsetX = randX + sectorStartU;
  std::cout << this->scale << " " << sectorScale <<  " " << offsetX << " " << offsetY << "\n";
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
    float * horizontalNeighbourChunk = 0;
    float * verticalNeighbourChunk = 0;
    float * diagonalNeighbourChunk = 0;

    if (offsetX < 0) {
      if (chunkCoordX == 0) {
        horizontalNeighbourChunkCoord = this->getCoordsToNeighbourLeft(-1, chunkCoordY, chunkScale);
        if (this->IsChunkAllocated(layer, horizontalNeighbourChunkCoord) == true) {
          horizontalNeighbourChunk = this->neighbourLeft[layer][horizontalNeighbourChunkCoord ]; 
        }
      }
      else {
        horizontalNeighbourChunkCoord = chunkCoordX - 1 + chunkCoordY * chunkScale;
        horizontalNeighbourChunk = this->realm[layer][horizontalNeighbourChunkCoord];
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
  // il dépasse du secteur à l'intérieur du royaume.
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
          // Le pixel courant est à l'intérieur du secteur courant
          else {
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
