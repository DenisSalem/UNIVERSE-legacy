#include <iostream>
#include "common.hpp"
#include "realm.hpp"
#include <cmath>

#include "png.hpp"

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
  delete [] this->realm[layer][chunkCoordX + chunkCoordY * (1 << layer)];
}

float * Realm::GetRealm(int layer, int chunkCoordX, int chunkCoordY) {
  return this->realm[layer][chunkCoordX + chunkCoordY * (1 << layer)];
}

float *** Realm::GetRealm() {
  return this->realm;
}

void Realm::Noise(int layer, int chunkCoordX, int chunkCoordY) {
  this->localChunk = this->realm[layer][chunkCoordX+chunkCoordY*(1<<layer)];
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

inline bool Realm::IsChunkACorner(int x, int y, int limit) {
  if (x == 0 && y == 0) {
    return true;
  }
  if (x == limit && y == 0) {
    return true;
  }
  if (x == limit && y == limit) {
    return true;
  }
  if (x == 0 && y == limit) {
    return true;
  }
  return false;
}

inline int Realm::DoesStampCrossCornerBeyondRealm(int offsetX, int offsetY, int chunkCoordX, int chunkCoordY, int sectorScale) {
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
  if(chunkCoordX == 0 && chunkCoordY == 0 && offsetX < 0 && offsetY < 0 && ceil(sqrt(pow(xCornerTopLeft,2) + pow(yCornerTopLeft,2))) <= stampRadius) {
      return CORNER_TOP_LEFT;
  }
  // Coin supérieur droit
  else if (chunkCoordX == this->chunkScale - 1 && chunkCoordY == 0 && offsetX+sectorScale >= this->scale && offsetY < 0 && ceil(sqrt(pow(xCornerTopRight,2) + pow(yCornerTopRight,2))) <= stampRadius) {
    return CORNER_TOP_RIGHT;
  }
  // Coin inférieur droit
  else if (chunkCoordX == this->chunkScale - 1 && chunkCoordY == this->chunkScale - 1 && offsetX+sectorScale >= this->scale && offsetY+sectorScale >= this->scale && ceil(sqrt(pow(xCornerBottomRight,2) + pow(yCornerBottomRight,2))) <= stampRadius) {
    return CORNER_BOTTOM_RIGHT;
  }
  // Coin inférieur gauche
  else if (chunkCoordX == 0 && chunkCoordY == this->chunkScale - 1 && offsetX < 0 && offsetY+sectorScale >= this->scale && ceil(sqrt(pow(xCornerBottomLeft,2) + pow(yCornerBottomLeft,2))) <= stampRadius) {
    return CORNER_BOTTOM_LEFT;
  }
  return false;
}

inline void Realm::StampWithinChunk(int x, int y, int stampIndex) {
  this->chunkIndex = y * this->scale + x;
  this->localChunk[ this->chunkIndex ] += this->sign * this->stamp[ stampIndex ] / this->step;
  this->UpdateMinMax(this->chunkIndex, this->localChunk);
}

inline void Realm::StampBeyondCorner(int x, int y, int chunkCoordX, int chunkCoordY, int chunkLimit, int stampIndex) {
  if (this->diagonalNeighbourChunk != 0) {
    if(x < 0 &&  y < 0) {
      if (chunkCoordX == 0) {
        this->chunkIndex = this->GetCoordsToNeighbourLeftTopLeft(x, y, this->scale) ;
      }
      else if (chunkCoordY == 0) {
        this->chunkIndex = this->GetCoordsToNeighbourTopTopLeft(x, y, this->scale) ;
      }
      else {
        this->chunkIndex = this->scale + x + this->scale * (this->scale + y);
      }
    }
    else if(x >= this->scale &&  y < 0) {
      if (chunkCoordX == chunkLimit) {
        this->chunkIndex = this->GetCoordsToNeighbourRightTopRight(x, y, this->scale) ;
      }
      else if (chunkCoordY == 0) {
        this->chunkIndex = this->GetCoordsToNeighbourTopTopRight(x, y, this->scale) ;
      }
      else {
        this->chunkIndex = x - this->scale + this->scale * (this->scale + y);
      }
    }
    else if(x >= this->scale && y >= this->scale) {
      if (chunkCoordX == chunkLimit) {
        this->chunkIndex = this->GetCoordsToNeighbourRightBottomRight(x, y, this->scale) ;
      }
      else if(chunkCoordY == chunkLimit) {
        this->chunkIndex = this->GetCoordsToNeighbourBottomBottomRight(x, y, this->scale) ;
      }
      else {
        this->chunkIndex = x - this->scale + this->scale * (y - this->scale);
      }
    }
    else if(x < 0 && y >= this->scale) {
      if (chunkCoordX == 0) {
        this->chunkIndex = this->GetCoordsToNeighbourLeftBottomLeft(x, y, this->scale);
      }
      else if (chunkCoordY == chunkLimit) {
        this->chunkIndex = this->GetCoordsToNeighbourBottomBottomLeft(x, y, this->scale) ;
      }
      else {
        this->chunkIndex = this->scale + x + this->scale * (y - this->scale);
      }
    }
    else {
      return;
    }
    this->diagonalNeighbourChunk[this->chunkIndex] += this->sign * this->stamp[ stampIndex ] / this->step;
    this->UpdateMinMax(chunkIndex, this->diagonalNeighbourChunk);
  }
}

inline void Realm::StampBeyondBorder(int x, int y, int chunkCoordX, int chunkCoordY, int stampIndex) {
  if(this->horizontalNeighbourChunk != 0 && x < 0 && y >= 0 && y < this->scale) {
    if (chunkCoordX == 0) {
      this->chunkIndex = this->GetCoordsToNeighbourLeft(x, y, this->scale) ;
    }
    else {
      this->chunkIndex = y * this->scale + this->scale + x;
    }
    horizontalNeighbourChunk[chunkIndex] += this->sign * this->stamp[ stampIndex ] / this->step;
    this->UpdateMinMax(chunkIndex, this->horizontalNeighbourChunk);
  }
  else if (this->horizontalNeighbourChunk != 0 && x >= this->scale && y >= 0 && y < this->scale) {
    if (chunkCoordX == this->chunkScale - 1) {
      this->chunkIndex = this->GetCoordsToNeighbourRight(x, y, this->scale) ;
    }
    else {
      this->chunkIndex = (y) * this->scale + x - this->scale;
    }
    horizontalNeighbourChunk[chunkIndex] += this->sign * this->stamp[ stampIndex ] / this->step;
    this->UpdateMinMax(chunkIndex, this->horizontalNeighbourChunk);
  }
  else if (this->verticalNeighbourChunk != 0 && y >= this->scale && x >= 0 && x < this->scale) {
    if (chunkCoordY == this->chunkScale - 1) {
      this->chunkIndex = this->GetCoordsToNeighbourBottom( x, y, this->scale) ;
    }
    else {
      this->chunkIndex = (y-this->scale) * this->scale + x;
    }
    this->verticalNeighbourChunk[ chunkIndex ] += this->sign * this->stamp[ stampIndex ] / this->step;
    this->UpdateMinMax(chunkIndex, this->verticalNeighbourChunk);
  }
  else if (this->verticalNeighbourChunk != 0 && y < 0 && x >= 0 && x < this->scale) {
    if (chunkCoordY == 0) {
      this->chunkIndex = this->GetCoordsToNeighbourTop(x, y, this->scale) ;
    }
    else {
      this->chunkIndex = (this->scale + y) * this->scale + x;
    }
    verticalNeighbourChunk[ chunkIndex ] += this->sign * this->stamp[ stampIndex ] / this->step;
    this->UpdateMinMax(chunkIndex, this->verticalNeighbourChunk);
  }
}

inline void Realm::TryToSetDestination(int chunkIndex, int layer, float *** origin, float ** destination) {
  if (this->IsChunkAllocated(origin, layer, chunkIndex) == true) {
    *destination = origin[layer][chunkIndex];
  }
}

// Selon le cas de figure, le dépassement du tampon peut déborder sur au plus trois chunk:
// Pour éviter de déréférencer 10 milles fois, on prépare les destinations en fonctions des paramétres
// de la récursion courante.
//
// On se propose donc deux fonctions pour préparer tout ça.

inline void Realm::PrepareDestinationOnCorner(int layer, int chunkCoordX, int chunkCoordY, int offsetX, int offsetY, int sectorScale) {
  this->diagonalNeighbourChunk = 0;
  // Dépassement sur le coin supérieur gauche
  if (offsetX < 0 && offsetY < 0) {
    if(chunkCoordX == 0) {
      this->TryToSetDestination(this->GetCoordsToNeighbourLeft(-1, chunkCoordY-1, this->chunkScale), layer, this->neighbourLeft, &(this->diagonalNeighbourChunk));     
    }
    else if(chunkCoordY == 0) {
      this->TryToSetDestination(this->GetCoordsToNeighbourTop(chunkCoordX-1, -1, this->chunkScale), layer, this->neighbourTop, &(this->diagonalNeighbourChunk));     
    }
    else {
      this->TryToSetDestination(chunkCoordX-1 + (chunkCoordY-1) * this->chunkScale, layer, this->realm, &(this->diagonalNeighbourChunk));     
    }
  }
  // Dépassement sur le coin supérieur droit
  else if (offsetX+sectorScale >= this->scale && offsetY < 0) {
    if(chunkCoordX == this->chunkScale - 1) {
      this->TryToSetDestination(this->GetCoordsToNeighbourRight(chunkCoordX+1, chunkCoordY-1, this->chunkScale), layer, this->neighbourRight, &(this->diagonalNeighbourChunk));     
    }
    else if(chunkCoordY == 0) {
      this->TryToSetDestination(this->GetCoordsToNeighbourTop(chunkCoordX+1, -1, this->chunkScale), layer, this->neighbourTop, &(this->diagonalNeighbourChunk));     
    }
    else {
      this->TryToSetDestination(chunkCoordX + 1 + (chunkCoordY - 1) * this->chunkScale, layer, this->realm, &(this->diagonalNeighbourChunk));     
    }
  }
  // Dépassement sur le coin inférieur droit
  else if (offsetX+sectorScale >= this->scale && offsetY+sectorScale >= this->scale) {
    if(chunkCoordX == this->chunkScale - 1) {
      this->TryToSetDestination(this->GetCoordsToNeighbourRight(chunkCoordX+1, chunkCoordY+1, this->chunkScale), layer, this->neighbourRight, &(this->diagonalNeighbourChunk));     
    }
    else if (chunkCoordY == this->chunkScale - 1) {
      this->TryToSetDestination(this->GetCoordsToNeighbourBottom(chunkCoordX+1, chunkCoordY+1, this->chunkScale), layer, this->neighbourBottom, &(this->diagonalNeighbourChunk));     
    }
    else {
      this->TryToSetDestination(chunkCoordX + 1 + (chunkCoordY + 1) * this->chunkScale, layer, this->realm, &(this->diagonalNeighbourChunk));     
    }
  }
  // Dépassement sur le coin inférieur gauche
  else if (offsetX < 0 && offsetY+sectorScale >= this->scale) {
    if(chunkCoordX == 0) {
      this->TryToSetDestination(this->GetCoordsToNeighbourLeft(chunkCoordX-1, chunkCoordY+1, this->chunkScale), layer, this->neighbourLeft, &(this->diagonalNeighbourChunk));     
    }
    else if(chunkCoordY == this->chunkScale - 1) {
      this->TryToSetDestination(this->GetCoordsToNeighbourBottom(chunkCoordX-1, chunkCoordY+1, this->chunkScale), layer, this->neighbourBottom, &(this->diagonalNeighbourChunk));     
    }
    else {
      this->TryToSetDestination(chunkCoordX - 1 + (chunkCoordY + 1) * this->chunkScale, layer, this->realm, &(this->diagonalNeighbourChunk));     
    }
  }
}

inline void Realm::PrepareDestinationOnBorder(int layer, int chunkCoordX, int chunkCoordY, int offsetX, int offsetY, int sectorScale) {
  this->horizontalNeighbourChunkCoord = 0;
  this->verticalNeighbourChunkCoord = 0;

  // Dépassement sur la gauche
  if (offsetX < 0) {
    if (chunkCoordX == 0) {
      this->TryToSetDestination(this->GetCoordsToNeighbourLeft(-1, chunkCoordY, this->chunkScale), layer, this->neighbourLeft, &(this->horizontalNeighbourChunk));     
    }
    else {
      this->TryToSetDestination(chunkCoordX - 1 + chunkCoordY * this->chunkScale, layer, this->realm, &(this->horizontalNeighbourChunk));     
    }
  }

  //Dépassement sur la droite
  else if (offsetX+sectorScale >= this->scale) {
    if (chunkCoordX == this->chunkScale - 1) {
      this->TryToSetDestination(this->GetCoordsToNeighbourRight(chunkCoordX+1, chunkCoordY, this->chunkScale), layer, this->neighbourRight, &(this->horizontalNeighbourChunk));     
    }
    else {
      this->TryToSetDestination(chunkCoordX + 1 + chunkCoordY * this->chunkScale, layer, this->realm, &(this->horizontalNeighbourChunk));     
    }
  }
  //Dépassement vers le bas
  if (offsetY+sectorScale >= this->scale) {
    if (chunkCoordY == this->chunkScale - 1) {
      this->TryToSetDestination(this->GetCoordsToNeighbourBottom(chunkCoordX, chunkCoordY+1, this->chunkScale), layer, this->neighbourBottom, &(this->verticalNeighbourChunk));     
    }
    else {
      this->TryToSetDestination(chunkCoordX + (chunkCoordY+1) * this->chunkScale, layer, this->realm, &(this->verticalNeighbourChunk));     
    }

  }
  //Dépassement vers le haut
  else if (offsetY < 0 ) {
    if (chunkCoordY == 0) {
      this->TryToSetDestination(this->GetCoordsToNeighbourTop(chunkCoordX, -1, this->chunkScale), layer, this->neighbourTop, &(this->verticalNeighbourChunk));     
    }
    else {
      this->TryToSetDestination(chunkCoordX + (chunkCoordY-1) * this->chunkScale, layer, this->realm, &(this->verticalNeighbourChunk));     
    }
  }
}
inline double Realm::SetAngle( int stampCrossCornerBeyondRealm, int StampCenterCoordY, int StampCenterCoordX) {
    if(stampCrossCornerBeyondRealm == CORNER_TOP_LEFT) {
      return atan2( (double) -StampCenterCoordY, (double) StampCenterCoordX );
    }
    else if(stampCrossCornerBeyondRealm == CORNER_TOP_RIGHT) {
      return atan2( (double) -StampCenterCoordY,(double) -StampCenterCoordX );
    }
    else if(stampCrossCornerBeyondRealm == CORNER_BOTTOM_RIGHT) {
      return atan2( (double) StampCenterCoordY,(double) -StampCenterCoordX );
    }
    else if(stampCrossCornerBeyondRealm == CORNER_BOTTOM_LEFT) {
      return atan2( (double) StampCenterCoordY, (double) StampCenterCoordX );
    }
}

void Realm::StampBeyondCornerBeyondRealm(int sectorScale, int halfScale, int offsetX, int offsetY, int stampCrossCornerBeyondRealm) {
    double distanceFromStampCenterToCorner;
    double angleStampCenterCorner;
    double newAngle;
    double currentPixelDistance;
    double scaledCurrentPixelDistance;
    double currentPixelAngle;
    float * transitionSpace;
    int StampCenterCoordX;
    int StampCenterCoordY;
    int newStampOffsetX;
    int newStampOffsetY;
    int newX;
    int newY;
    int transitionSpaceScale = this->scale << 2;
    int halfTransitionSpaceScale = this->scale << 1;
    int stampX = 0;
    int stampY = 0;
    int stampIndex;
    int chunkIndex;
    double sectorScaleMinus1 = (double) (sectorScale-1);


    // Préparation des coordonnées de référence.
    transitionSpace = new float [transitionSpaceScale * transitionSpaceScale]();
    StampCenterCoordX = halfScale - ABS(offsetX % sectorScale);
    StampCenterCoordY = halfScale - ABS(offsetY % sectorScale);
    distanceFromStampCenterToCorner = sqrtl(pow(StampCenterCoordX,2) + pow(StampCenterCoordY,2))*this->step;
    angleStampCenterCorner = this->SetAngle(stampCrossCornerBeyondRealm, StampCenterCoordY, StampCenterCoordX);

    // Préparation des coordonnées de transition
    if ( stampCrossCornerBeyondRealm == CORNER_TOP_LEFT) {
      newAngle = - PI2_12 + (( angleStampCenterCorner) * PI2_3) / PI_2 ;
    }
    else if(stampCrossCornerBeyondRealm == CORNER_TOP_RIGHT) {
      newAngle = -PI2_12 + (( angleStampCenterCorner + PI_2) * PI2_3) / PI_2 ;
    }
    else if ( stampCrossCornerBeyondRealm == CORNER_BOTTOM_LEFT || stampCrossCornerBeyondRealm == CORNER_BOTTOM_RIGHT) {
      newAngle = - PI2_12 + (( angleStampCenterCorner) * PI2_3) / PI_2 ;
    }

    newStampOffsetX = (int) halfTransitionSpaceScale - (this->scale >> 1) + cos(newAngle) * distanceFromStampCenterToCorner ;
    newStampOffsetY = (int) halfTransitionSpaceScale - (this->scale >> 1) - sin(newAngle) * distanceFromStampCenterToCorner ;


    // Copie du tampon sur l'espace de transition
    for (int y=0; y < this->scale; y++) {
      stampX = 0;
      for (int x=0; x < this->scale; x++) {
        stampIndex = stampX + this->scale * stampY;
        if ( stamp[ stampIndex ] != 0) {
          transitionSpace[x+newStampOffsetX + (y + newStampOffsetY) * transitionSpaceScale] = stamp[ stampIndex ];
        }
        stampX += 1;
      }
      stampY += 1;
    }
    
    // DEBUG //////////////////////////////////////////////////
    // 
    //
    /*
    
    unsigned char color;
    PIXEL ** tsPreview = new PIXEL *[transitionSpaceScale];
    for (int x = 0; x < transitionSpaceScale; x++) {
      tsPreview[x] = new PIXEL[transitionSpaceScale];
      for (int y = 0; y < transitionSpaceScale; y++) {
          color = (unsigned char) (255 * (transitionSpace[ x + y * transitionSpaceScale ] ) );
          if(color == 0 && sqrtl(pow(x-(this->scale<<1),2) + pow(y- (this->scale<<1),2)) < (this->scale << 1))
            color = 128;
          tsPreview[x][y].Red	= color;
          tsPreview[x][y].Green = color;
          tsPreview[x][y].Blue = color;
      }
    }
    
    writePng(tsPreview,transitionSpaceScale,transitionSpaceScale,"transitionSpace.png");
                                                             */
                                                             //
                                                             //
    // END DEBUG //////////////////////////////////////////////
    if( stampCrossCornerBeyondRealm == CORNER_TOP_RIGHT) {
      for(int x = 0; x < sectorScale; x++) {
        for(int y = 0; y < sectorScale; y++) {
          // ROYAUME COURANT
          currentPixelDistance = sqrtl( pow( sectorScale-x,2) + pow(-y-1,2) );
          // Pour économiser un peu de temps CPU on évite des calcules inutiles en s'assurant
          // que le pixel courant est dans une zone d'impression qui fait sens.
          // On réitérera ce test plus bas.
          if(currentPixelDistance < sectorScale) {
            scaledCurrentPixelDistance = currentPixelDistance * this->step;
            // À surveiller: les coordonnées passées à atan2 ne doivent jamais être nulles,
            // sinon cela produit des artefacts. D'où le -y-0.0001.
            currentPixelAngle = atan2((double) -y-1, (double) -sectorScale+x);
            newAngle = - PI2_12 + (( currentPixelAngle + PI_2) * PI2_3) / PI_2 ;
            newX = (int) (cos(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            newY = (int) (-sin(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            chunkIndex = this->scale - sectorScale + x + this->scale * y;
            this->localChunk[ chunkIndex ] += this->sign * transitionSpace[newX + newY * transitionSpaceScale] / this->step;
            this->UpdateMinMax(chunkIndex, this->localChunk);
          }

          // ROYAUME ADJACENT DROIT
          currentPixelDistance = sqrtl( pow(x,2) + pow(-y-0.0001,2) );
          if(currentPixelDistance < sectorScale) { 
            scaledCurrentPixelDistance = currentPixelDistance * this->step;
            currentPixelAngle = atan2((double) -y-0.0001, (double) x+0.0001);
            newAngle = PI_2 + (currentPixelAngle * PI2_3) / PI_2 ;
            newX = (int) (cos(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            newY = (int) (-sin(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            chunkIndex = this->GetCoordsToNeighbourRight(this->scale+x, y,this->scale);
            this->horizontalNeighbourChunk[ chunkIndex ] += this->sign * transitionSpace[newX + newY * transitionSpaceScale] / this->step;
            this->UpdateMinMax(chunkIndex, this->horizontalNeighbourChunk);
          }

          // ROYAUME ADJACENT SUPÉRIEUR
          currentPixelDistance = sqrtl( pow(-sectorScaleMinus1+x,2) + pow(sectorScaleMinus1-y,2) );
          if(currentPixelDistance < sectorScale) { 
            scaledCurrentPixelDistance = currentPixelDistance * this->step;
            currentPixelAngle = atan2((double) -y+sectorScaleMinus1, (double) x-sectorScaleMinus1);
            newAngle = PI_2 + (( currentPixelAngle - PI_2) * PI2_3) / PI_2 ;
            newX = (int) (cos(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            newY = (int) (-sin(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            chunkIndex = this->GetCoordsToNeighbourTop(this->scale - sectorScale + x, y-sectorScale, this->scale);
            this->verticalNeighbourChunk[ chunkIndex ] += this->sign * transitionSpace[newX + newY * transitionSpaceScale] / this->step;
            this->UpdateMinMax(chunkIndex, this->verticalNeighbourChunk);
          }
        }
      }
    }
    else if( stampCrossCornerBeyondRealm == CORNER_TOP_LEFT) {
      for(int x = 0; x < sectorScale; x++) {
        for(int y = 0; y < sectorScale; y++) {
          // ROYAUME COURANT
          currentPixelDistance = sqrtl( pow(x,2) + pow(-y-1,2) );
          if(currentPixelDistance < sectorScale) {
            scaledCurrentPixelDistance = currentPixelDistance * this->step;
            currentPixelAngle = atan2((double) -y-1, (double) x);
            newAngle = - PI2_12 + (( currentPixelAngle) * PI2_3) / PI_2 ;
            newX = (int) (cos(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            newY = (int) (-sin(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            chunkIndex = x + this->scale * y;
            this->localChunk[ chunkIndex ] += this->sign * transitionSpace[newX + newY * transitionSpaceScale] / this->step;
            this->UpdateMinMax(chunkIndex, this->localChunk);
          }

          // ROYAUME ADJACENT GAUCHE
          currentPixelDistance = sqrtl( pow(sectorScaleMinus1-x,2) + pow(-y-0.0001,2) );
          if(currentPixelDistance < sectorScale) { 
            scaledCurrentPixelDistance = currentPixelDistance * this->step;
            currentPixelAngle = atan2((double) -y-0.0001, (double) -sectorScaleMinus1+x);
            newAngle =  PI + PI2_12 + (( currentPixelAngle + PI_2) * PI2_3) / PI_2 ;
            newX = (int) (cos(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            newY = (int) (-sin(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            chunkIndex = this->GetCoordsToNeighbourLeft(-sectorScale+x, y,this->scale);
            this->horizontalNeighbourChunk[ chunkIndex ] += this->sign * transitionSpace[newX + newY * transitionSpaceScale] / this->step;
            this->UpdateMinMax(chunkIndex, this->horizontalNeighbourChunk);
          }

          // ROYAUME ADJACENT SUPÉRIEUR
          currentPixelDistance = sqrtl(pow(x,2) + pow(sectorScaleMinus1-y,2) );
          if(currentPixelDistance < sectorScale) { 
            scaledCurrentPixelDistance = currentPixelDistance * this->step;
            currentPixelAngle = atan2((double) sectorScaleMinus1-y, (double) x);
            newAngle = - PI2_12 + (( currentPixelAngle) * PI2_3) / PI_2 ;
            newX = (int) (cos(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            newY = (int) (-sin(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            chunkIndex = this->GetCoordsToNeighbourTop(x, -sectorScale+y, this->scale);
            this->verticalNeighbourChunk[ chunkIndex ] += this->sign * transitionSpace[newX + newY * transitionSpaceScale] / this->step;
            this->UpdateMinMax(chunkIndex, this->verticalNeighbourChunk);
          }
        }
      }
    }
    else if( stampCrossCornerBeyondRealm == CORNER_BOTTOM_LEFT) {
      for(int x = 0; x < sectorScale; x++) {
        for(int y = 0; y < sectorScale; y++) {
          // ROYAUME COURANT
          currentPixelDistance = sqrtl( pow(x,2) + pow(sectorScaleMinus1-y,2) );
          if(currentPixelDistance < sectorScale) {
            scaledCurrentPixelDistance = currentPixelDistance * this->step;
            currentPixelAngle = atan2((double) -y+sectorScaleMinus1, (double) x);
            newAngle = - PI2_12 + (( currentPixelAngle) * PI2_3) / PI_2 ;
            newX = (int) ( cos(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            newY = (int) ( -sin(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            chunkIndex = x + this->scale * (this->scale - sectorScale + y);
            this->localChunk[ chunkIndex ] += this->sign * transitionSpace[newX + newY * transitionSpaceScale] / this->step;
            this->UpdateMinMax(chunkIndex, this->localChunk);
          }

          // ROYAUME ADJACENT GAUCHE
          currentPixelDistance = sqrtl( pow(-sectorScale+1+x, 2) + pow(sectorScale-y, 2) );
          if(currentPixelDistance < sectorScale) {
            scaledCurrentPixelDistance = currentPixelDistance * this->step;
            currentPixelAngle = atan2((double) (sectorScale-y), (double) (-sectorScale+1+x));
            newAngle = PI_2 + (( currentPixelAngle-PI_2) * PI2_3) / PI_2 ;
            newX = (int) (  cos(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            newY = (int) ( -sin(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            chunkIndex = this->GetCoordsToNeighbourLeft( -sectorScale + x, this->scale - sectorScale + y,this->scale);
            this->horizontalNeighbourChunk[ chunkIndex ] += this->sign * transitionSpace[newX + newY * transitionSpaceScale] / this->step;
            this->UpdateMinMax(chunkIndex, this->horizontalNeighbourChunk);
          }

          // ROYAUME ADJACENT INFÉRIEUR
          currentPixelDistance = sqrtl( pow(x, 2) + pow(y, 2) );
          if(currentPixelDistance < sectorScale) {
            scaledCurrentPixelDistance = currentPixelDistance * this->step;
            currentPixelAngle = atan2((double) -y-1, (double) x+1);
            newAngle = - PI2_12 + (( currentPixelAngle ) * PI2_3) / PI_2 ;
            newX = (int) (  cos(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            newY = (int) ( -sin(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            chunkIndex = this->GetCoordsToNeighbourBottom(x, this->scale+y, this->scale);
            this->verticalNeighbourChunk[ chunkIndex ] += this->sign * transitionSpace[newX + newY * transitionSpaceScale] / this->step;
            this->UpdateMinMax(chunkIndex, this->verticalNeighbourChunk);
          }
        }
      }
    }
    else { // if( stampCrossCornerBeyondRealm == CORNER_BOTTOM_RIGHT)
      for(int x = 0; x < sectorScale; x++) {
        for(int y = 0; y < sectorScale; y++) {
          // ROYAUME COURANT
          currentPixelDistance = sqrtl( pow(sectorScaleMinus1-x,2) + pow(sectorScaleMinus1-y,2) );
          if(currentPixelDistance < sectorScale) {
            scaledCurrentPixelDistance = currentPixelDistance * this->step;
            currentPixelAngle = atan2((double) sectorScaleMinus1-y, (double) -sectorScaleMinus1+x);
            newAngle = PI_2 + (( currentPixelAngle - PI_2) * PI2_3) / PI_2;
            newX = (int) ( cos(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            newY = (int) ( -sin(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            chunkIndex = this->scale - sectorScale + x + this->scale * (this->scale - sectorScale + y);
            this->localChunk[ chunkIndex ] += this->sign * transitionSpace[newX + newY * transitionSpaceScale] / this->step;
            this->UpdateMinMax(chunkIndex, this->localChunk);
          }

          // ROYAUME ADJACENT DROIT
          currentPixelDistance = sqrtl( pow(x, 2) + pow(sectorScaleMinus1-y, 2) );
          if(currentPixelDistance < sectorScale) {
            scaledCurrentPixelDistance = currentPixelDistance * this->step;
            currentPixelAngle = atan2((double) (sectorScale+1-y), (double) (x));
            newAngle = - PI2_12 + (( currentPixelAngle) * PI2_3) / PI_2 ;
            newX = (int) (  cos(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            newY = (int) ( -sin(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            chunkIndex = this->GetCoordsToNeighbourRight(this->scale+x, (this->scale - sectorScale + y),this->scale);
            this->horizontalNeighbourChunk[ chunkIndex ] += this->sign * transitionSpace[newX + newY * transitionSpaceScale] / this->step;
            this->UpdateMinMax(chunkIndex, this->horizontalNeighbourChunk);
          }

          // ROYAUME ADJACENT INFÉRIEUR
          currentPixelDistance = sqrtl( pow(-sectorScaleMinus1+x, 2) + pow(-y-1, 2) );
          if(currentPixelDistance < sectorScale) {
            scaledCurrentPixelDistance = currentPixelDistance * this->step;
            currentPixelAngle = atan2((double) -y-1, (double) -sectorScaleMinus1+x);
            newAngle = - PI2_12 + (( currentPixelAngle + PI_2 ) * PI2_3) / PI_2 ;
            newX = (int) (  cos(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            newY = (int) ( -sin(newAngle) * scaledCurrentPixelDistance + halfTransitionSpaceScale);
            chunkIndex = this->GetCoordsToNeighbourBottom(this->scale - sectorScale + x, this->scale+y, this->scale);
            this->verticalNeighbourChunk[ chunkIndex ] += this->sign * transitionSpace[newX + newY * transitionSpaceScale] / this->step;
            this->UpdateMinMax(chunkIndex, this->verticalNeighbourChunk);
          }
        }
      }
    }
    return;
}

void Realm::Noise(int layer, int chunkCoordX, int chunkCoordY, int sectorScale, int sectorStartU, int sectorStartV) {
  if (sectorScale == 1) {
    return;
  }

  this->sign = getRandom() & 1 ? 1.0 : -1.0;
  this->step = this->scale / sectorScale;
  this->horizontalNeighbourChunk=0;
  this->verticalNeighbourChunk=0;
  this->diagonalNeighbourChunk=0;

  // Cette curieuse formule permet de connaitre pour un calque donné la dimension
  // d'un morceau courant. Habile!
  this->chunkScale = 1 << layer;

  int halfScale = sectorScale >> 1;
  int randX = - halfScale + getRandom() % (sectorScale);
  int randY = - halfScale + getRandom() % (sectorScale);
  int stampId =  getRandom() % this->stampCount;
  int stampIndex;
  int chunkLimit;
  int offsetX = randX + sectorStartU;
  int offsetY = randY + sectorStartV;
  int stampX=0,stampY=0;
  int stampCrossCornerBeyondRealm = this->DoesStampCrossCornerBeyondRealm(offsetX, offsetY, chunkCoordX, chunkCoordY, sectorScale);
  int localChunkCoord = chunkCoordX + chunkCoordY * chunkScale;
  this->stamp = this->stamps[stampId];
  this->PrepareDestinationOnCorner(layer, chunkCoordX, chunkCoordY,offsetX,offsetY,sectorScale);
  this->PrepareDestinationOnBorder(layer, chunkCoordX, chunkCoordY,offsetX,offsetY,sectorScale);
  if ( this->IsChunkACorner(chunkCoordX,chunkCoordY,chunkScale-1) && stampCrossCornerBeyondRealm) {
    this->StampBeyondCornerBeyondRealm(sectorScale, halfScale, offsetX, offsetY, stampCrossCornerBeyondRealm);
  }
  else {
    chunkLimit = chunkScale - 1;
    for (int y=0; y<sectorScale; y++) {
      stampX = 0;
      for (int x=0; x<sectorScale; x++) {
        stampIndex = stampX + sectorScale * stampY;
	if ( stamp[ stampIndex ] != 0) {
          if (x+offsetX >= 0 && x+offsetX < this->scale && y+offsetY >= 0 && y+offsetY < this->scale) {
            this->StampWithinChunk(x+offsetX, y+offsetY, stampIndex);
          }
          else {
            StampBeyondCorner(x+offsetX, y+offsetY, chunkCoordX, chunkCoordY, chunkLimit, stampIndex);
            StampBeyondBorder(x+offsetX, y+offsetY, chunkCoordX, chunkCoordY, stampIndex);
          }
        }
        stampX += this->step;
      }
      stampY += this->step * this->step;
    }
  }
  
  this->Noise(layer, chunkCoordX, chunkCoordY, halfScale, sectorStartU,		  sectorStartV);
  this->Noise(layer, chunkCoordX, chunkCoordY, halfScale, sectorStartU, 	  sectorStartV+halfScale);
  this->Noise(layer, chunkCoordX, chunkCoordY, halfScale, sectorStartU+halfScale, sectorStartV+halfScale);
  this->Noise(layer, chunkCoordX, chunkCoordY, halfScale, sectorStartU+halfScale, sectorStartV);
};
