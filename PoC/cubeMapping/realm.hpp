#ifndef _REALM_
#define _REALM_

#include "common.hpp"

class Realm {
  public:
    Realm(int LoD, float * min, float * max);

    void SetNeighbours(
      float ** neighbourTop[MAXIMUM_NUMBER_OF_LAYERS],
      float ** neighbourBottom[MAXIMUM_NUMBER_OF_LAYERS],
      float ** neighbourLeft[MAXIMUM_NUMBER_OF_LAYERS],
      float ** neightbourRight[MAXIMUM_NUMBER_OF_LAYERS]
    );

    float * GetRealm(int layer, int chunkCoordX, int chunkCoordY);
    void AddStamp(float * stamp);
    void AllocateChunk(int layer, int chunkCoordX, int chunkCoordY);
    void DeallocateChunk(int layer, int chunkCoordX, int chunkCoordY);
    void Noise(int layer, int chunkCoordX, int chunkCoordY);

  private:
    void Noise(int layer, int chunkCoordX, int chunkCoordY, int sectorScale, int sectorStartU, int sectorStartV);

    virtual int getCoordsToNeighbourTop(int x, int y, int scale) = 0;
    virtual int getCoordsToNeighbourBottom(int x, int y, int scale) = 0; 
    virtual int getCoordsToNeighbourLeft(int x, int y, int scale) = 0; 
    virtual int getCoordsToNeighbourRight(int x, int y, int scale) = 0; 

    int stampCount;
    int scale;
    int area;
    int LoD;

    float * min;
    float * max;

    float * stamps[MAXIMUM_NUMBER_OF_STAMPS];

    float ** realm[MAXIMUM_NUMBER_OF_LAYERS];
    float ** neighbourTop[MAXIMUM_NUMBER_OF_LAYERS];
    float ** neighbourBottom[MAXIMUM_NUMBER_OF_LAYERS];
    float ** neighbourLeft[MAXIMUM_NUMBER_OF_LAYERS];
    float ** neightbourRight[MAXIMUM_NUMBER_OF_LAYERS];
};
#endif
