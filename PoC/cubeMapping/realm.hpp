#ifndef _REALM_
#define _REALM_

#include "common.hpp"

class Realm {
  public:
    Realm(int LoD, float * min, float * max);

    void SetNeighbours(
      float *** neighbourTop,
      float *** neighbourBottom,
      float *** neighbourLeft,
      float *** neightbourRight
    );

    float * GetRealm(int layer, int chunkCoordX, int chunkCoordY);
    float *** GetRealm();
    void AddStamp(float * stamp);
    bool IsChunkAllocated(float *** dest, int layer, int chunkIndex);
    void AllocateChunk(int layer, int chunkCoordX, int chunkCoordY);
    void DeallocateChunk(int layer, int chunkCoordX, int chunkCoordY);
    void Noise(int layer, int chunkCoordX, int chunkCoordY);

  protected:
    void UpdateMinMax(int chunkIndex, float * chunk);
    int scale;

  private:
    bool DoesStampCrossCorner(int offsetX, int offsetY, int sectorScale);
    void Noise(int layer, int chunkCoordX, int chunkCoordY, int sectorScale, int sectorStartU, int sectorStartV);
    virtual int getCoordsToNeighbourTop(int x, int y, int scale) = 0;
    virtual int getCoordsToNeighbourBottom(int x, int y, int scale) = 0; 
    virtual int getCoordsToNeighbourLeft(int x, int y, int scale) = 0; 
    virtual int getCoordsToNeighbourRight(int x, int y, int scale) = 0; 

    int stampCount;
    int area;
    int LoD;

    float * min;
    float * max;

    float * stamps[MAXIMUM_NUMBER_OF_STAMPS];

    float *** realm;
    float *** neighbourTop;
    float *** neighbourBottom;
    float *** neighbourLeft;
    float *** neighbourRight;
};
#endif
