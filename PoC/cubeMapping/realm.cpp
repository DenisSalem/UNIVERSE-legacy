#include <iostream>
#include "common.hpp"
#include "realm.hpp"

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
  int offsetY = randY + sectorStartV;
  int offsetX = randX + sectorStartU;
  int stampX=0,stampY=0;
  double distanceFromStampCenterToCurrentCorner; // Whoaw, what a long name, whoaw.

  // Cette curieuse formule permet de connaitre pour un calque donné la dimension
  // d'un morceau courant. Habile!
  int chunkScale = 1 << layer;
  int localChunkCoord = chunkCoordX + chunkCoordY * chunkScale;

  float * localChunk= this->realm[layer][localChunkCoord];
  float * stamp = this->stamps[stampId];

  // Selon le cas de figure, le dépassement du tampon peut déborder sur au plus trois chunk:


  int horizontalNeighbourChunkCoord = 0;

  float * horizontalNeighbourChunk = 0;
  float * verticalNeighbourChunk = 0;
  float * diagonalNeighbourChunk = 0;
  float * horizontalLocalNeighbourChunk = 0;

  // Pour éviter de déréférencer 10 milles fois, on prépare les destinations en fonctions des paramétres
  // de la récursion courante.
  if (offsetX < 0) {
    if (chunkCoordX == 0) {
      if (this->neighbourLeft[layer] != 0) {
        if(this->neighbourLeft[layer][horizontalNeighbourChunkCoord ] != 0) {
          horizontalNeighbourChunkCoord = this->getCoordsToNeighbourLeft(-1, chunkCoordY, chunkScale);
          horizontalNeighbourChunk = this->neighbourLeft[layer][horizontalNeighbourChunkCoord ]; 
        }
      }
    }
    else {
      horizontalNeighbourChunkCoord = chunkCoordX - 1 + chunkCoordY * chunkScale;
      horizontalLocalNeighbourChunk = this->realm[layer][horizontalNeighbourChunkCoord];
    }
  }

  // Quand le tampon est exactement contenu à l'intérieur d'une face
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

  // Dépassement en X
  else if ( (offsetX < 0 || offsetX+sectorScale-1 >= this->scale) && ( offsetY >= 0 && offsetY+sectorScale-1 < this->scale)) { 
    for (int y=0; y<sectorScale; y++) {
      stampX = 0;
      for (int x=0; x<sectorScale; x++) {
        stampIndex = stampX + sectorScale * stampY;
	if ( stamp[ stampIndex ] != 0) {
          // Le morceau courant est à l'extrême gauche, il faut donc se reporter sur un autre royaume.
          if(chunkCoordX == 0) {
            //On regarde si le le morceau du royaume voisin est alloué, sinon on ne fait rien. Et ouais.
            if(horizontalNeighbourChunk != 0) {
              // On prépare la destination sur le côté gauche
              if(offsetX+x < 0) {
	        chunkIndex = this->getCoordsToNeighbourLeft( offsetX+x, y+offsetY, this->scale) ;
                horizontalNeighbourChunk[chunkIndex] += sign * stamp[ stampIndex ] / inc;
                this->UpdateMinMax(chunkIndex, horizontalNeighbourChunk);
              }
              // Ou sur le côté droit
              else {
	        chunkIndex = (y+offsetY) * this->scale + x+offsetX;
	        localChunk[ chunkIndex ] += sign * stamp[ stampIndex ] / inc;
                this->UpdateMinMax(chunkIndex, localChunk);
              }
            }
          }
          else if (chunkCoordX == chunkScale - 1 ) {
          }
          // Sinon c'est cool, on peut calculer la coordonnée relative facilement
          else {
            localChunk = this->realm[layer][chunkCoordX-1 + chunkCoordY * chunkScale];  
	    chunkIndex = (y+offsetY) * this->scale + this->scale - x+offsetX;
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
