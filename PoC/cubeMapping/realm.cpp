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
  this->realm[0] = new float*[1]();

  for (int i = 0; i < MAXIMUM_NUMBER_OF_LAYERS-1; i++) {
    this->realm[i+1] = new float*[ 2 << (i * 2 + 1) ]();
  }

  this->AllocateChunk(0,0,0); 
}

void Realm::AddStamp(float * stamp) {
  this->stamps[this->stampCount]  = stamp;
  this->stampCount++;
}

void Realm::AllocateChunk(int layer, int chunkCoordX, int chunkCoordY) {
  this->realm[layer][chunkCoordX + chunkCoordY * (2 << layer)] = new float[this->area]; 
}

void Realm::DeallocateChunk(int layer, int chunkCoordX, int chunkCoordY) {
  delete [] this->realm[layer][chunkCoordX + chunkCoordY * (2 << layer)];
}
float * Realm::GetRealm(int layer, int chunkCoordX, int chunkCoordY) {
  return this->realm[layer][chunkCoordX + chunkCoordY * (2 << layer)];
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
  int faceIndex;
  int offsetY = randY + sectorStartV;
  int offsetX = randX + sectorStartU;
  int stampX=0,stampY=0;
  double distanceFromStampCenterToCurrentCorner; // Whoaw, what a long name, whoaw.

  // Cette curieuse formule permet de connaitre pour un calque donné la dimension
  // d'un morceau courant. Habile!
  int chunkScale = 2 << layer;

  float * dest = this->realm[layer][chunkCoordX + chunkCoordY * chunkScale];
  float * stamp = this->stamps[stampId];

  // Selon le cas de figure, le dépassement du tampon peut déborder sur au plus trois chunk:
  float * verticalNeightbourChunk;
  float * horizontalNeightbourChunk;
  float * diagonalNeightbourChunk;
  int neighbourChunkCoord;

  // Quand le tampon est exactement contenu à l'intérieur d'une face
  if ( offsetY >= 0 && offsetY+sectorScale-1 < this->scale && offsetX >= 0 && offsetX+sectorScale-1< this->scale) {
    for (int y=0; y<sectorScale; y++) {
      stampX = 0;
      for (int x=0; x<sectorScale; x++) {
        stampIndex = stampX + sectorScale * stampY;
	if ( stamp[ stampIndex ] != 0) {
	  faceIndex = (y+offsetY) * this->scale + x+offsetX;
	  dest[ faceIndex ] += sign * stamp[ stampIndex ] / inc;
	  if (*this->max < dest[ faceIndex ]) {
	    *this->max = dest[ faceIndex ];
	  }
	  if (*this->min <= 0 && *this->min > dest[ faceIndex ]) {
	    *this->min = dest[ faceIndex ];
	  }
	}
	stampX += inc;
      }
      stampY += inc * inc;
    }
  }
  
  // Dépassement X
  else if ( !(offsetX >= 0 && offsetX+scale-1 < this->scale) && ( offsetY >= 0 && offsetY+scale-1 < this->scale)) { 
    for (int y=0; y<sectorScale; y++) {
      stampX = 0;
      for (int x=0; x<sectorScale; x++) {
        stampIndex = stampX + sectorScale * stampY;
	if ( stamp[ stampIndex ] != 0) {
          // On dépasse du morceau sur la gauche
          if (offsetX + x <0) {
            // Le morceau courant est à l'extrême gauche, il faut donc se reporter sur un autre royaume.
            if(chunkCoordX==0) {
              //On regarde si le le morceau du royaume voisin est alloué, sinon on ne fait rien. Et ouais.
              if (this->neighbourLeft[layer] != 0) {
                // ERREUR DE SEGMENTATION, LA COORDONNÉE EST BIDON
                neighbourChunkCoord = this->getCoordsToNeighbourLeft(-1, chunkCoordY, chunkScale);
                if(this->neighbourLeft[layer][neighbourChunkCoord ] != 0) {
                  horizontalNeightbourChunk = this->neighbourLeft[layer][neighbourChunkCoord];
	          faceIndex =  this->getCoordsToNeighbourLeft( -x+offsetX, y+offsetY, this->scale) ;
	          horizontalNeightbourChunk[ faceIndex ] += sign * stamp[ stampIndex ] / inc;
	          if (*this->max < horizontalNeightbourChunk[ faceIndex ]) {
	            *this->max = horizontalNeightbourChunk[ faceIndex ];
	          }
	          if (*this->min <= 0 && *this->min > dest[ faceIndex ]) {
	            *this->min = horizontalNeightbourChunk[ faceIndex ];
	          }
                }
              }
            }
            // Sinon c'est cool, on peut calculer la coordonnée relative facilement
            else {   
              horizontalNeightbourChunk = this->realm[layer][chunkCoordX-1 + chunkCoordY * chunkScale];  
	      faceIndex = (y+offsetY) * this->scale + this->scale - x+offsetX;
	      horizontalNeightbourChunk[ faceIndex ] += sign * stamp[ stampIndex ] / inc;
	      if (*this->max < horizontalNeightbourChunk[ faceIndex ]) {
	        *this->max = horizontalNeightbourChunk[ faceIndex ];
	      }
	      if (*this->min <= 0 && *this->min > dest[ faceIndex ]) {
	        *this->min = horizontalNeightbourChunk[ faceIndex ];
	      }
            }
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
