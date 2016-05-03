#include <iostream>
#include "common.hpp"
#include "realm.hpp"

Realm::Realm(int Lod) {
  this->LoD = LoD;
  this->scale = 2 << LoD;
  this->area = 2 << (LoD+LoD+1);

  this->realm[0] = new float*[1]();

  for (int i = 0; i < MAXIMUM_NUMBER_OF_LAYERS-1; i++) {
    this->realm[i+1] = new float*[ 2 << (i * 2 + 1) ]();
  }

  this->AllocateChunk(0,0);
}

void Realm::AllocateChunk(int layer, int chunk) {
  this->realm[layer][chunk] = new float[this->area]; 
}


void Realm::Noise(int layer, int chunkCoordX, int chunkCoordY) {
  this->Noise(layer, chunkCoordX, chunkCoordY, this->scale, 0, 0);
}

void Realm::Noise(int layer, int chunkCoordX, int chunkCoordY, int sectorScale, int sectorStartU, int sectorStartV) {
  if (scale == 1) {
    return;
  }

  float sign = getRandom() & 1 ? 1.0 : -1.0;

  int halfScale = scale >> 1;
  int randX = - halfScale + getRandom() % (scale);
  int randY = - halfScale + getRandom() % (scale);
  int stampId =  getRandom() % this->stampCounter;
  int inc = this->scale / scale;
  int stampIndex;
  int faceIndex;
  int offsetY = randY + sectorStartV;
  int offsetX = randX + sectorStartU;
  int stampX=0,stampY=0;

  float * dest = this->realm[layer][chunkCoordX + chunkCoordY * (2 << layer)];
  float * stamp = this->stamps[stampId];


  if ( offsetY >= 0 && offsetY+sectorScale-1 < this->scale && offsetX >= 0 && offsetX+sectorScale-1< this->scale) {
    for(int y=0; y<sectorScale;y++) {
      stampX = 0;
      for(int x=0; x<sectorScale;x++) {
        stampIndex = stampX + sectorScale * stampY;
	if( stamp[ stampIndex ] != 0) {
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
  
  this->Noise(layer, chunkCoordX, chunkCoordY, halfScale, sectorStartU,		  sectorStartV);
  this->Noise(layer, chunkCoordX, chunkCoordY, halfScale, sectorStartU, 	  sectorStartV+halfScale);
  this->Noise(layer, chunkCoordX, chunkCoordY, halfScale, sectorStartU+halfScale, sectorStartV+halfScale);
  this->Noise(layer, chunkCoordX, chunkCoordY, halfScale, sectorStartU+halfScale, sectorStartV);
};
