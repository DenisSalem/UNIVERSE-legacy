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
    void PrepareDestinationOnBorder(int layer, int chunkCoordX, int chunkCoordY, int offsetX, int offsetY, int sectorScale);
    void PrepareDestinationOnCorner(int layer, int chunkCoordX, int chunkCoordY, int offsetX, int offsetY, int sectorScale);
    void UpdateMinMax(int chunkIndex, float * chunk);
    void StampBeyondBorder(int x, int y, int chunkCoordX, int chunkCoordY, int stampIndex);
    //void CrossCorner();
    void StampWithinChunk(int x, int y, int stampIndex);
    bool DoesStampCrossCorner(int offsetX, int offsetY, int sectorScale);
    bool DoesStampCrossCornerWithinRealm(int x, int y, int limit);
    void Noise(int layer, int chunkCoordX, int chunkCoordY, int sectorScale, int sectorStartU, int sectorStartV);
    int scale;

  private:
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

    // Temporary space
    float * horizontalNeighbourChunk;
    float * verticalNeighbourChunk;
    float * diagonalNeighbourChunk;
    float * localChunk;
    float * stamp;
    float sign;
    int horizontalNeighbourChunkCoord;
    int verticalNeighbourChunkCoord;
    int chunkIndex;
    int chunkScale;
    int inc;
};
#endif
