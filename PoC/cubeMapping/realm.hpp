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
    void TryToSetDestination(int chunkIndex, int layer, float *** origin, float ** destination);
    void PrepareDestinationOnBorder(int layer, int chunkCoordX, int chunkCoordY, int offsetX, int offsetY, int sectorScale);
    void PrepareDestinationOnCorner(int layer, int chunkCoordX, int chunkCoordY, int offsetX, int offsetY, int sectorScale);
    void UpdateMinMax(int chunkIndex, float * chunk);
    void StampBeyondBorder(int x, int y, int chunkCoordX, int chunkCoordY, int stampIndex);
    void StampBeyondCorner(int x, int y, int chunkCoorX, int chunkCoordy, int chunkLimit, int stampIndex);
    void StampWithinChunk(int x, int y, int stampIndex);
    bool DoesStampCrossCornerBeyondRealm(int offsetX, int offsetY, int chunkCoordX, int chunkCoordY, int sectorScale);
    bool IsChunkACorner(int x, int y, int limit);
    void Noise(int layer, int chunkCoordX, int chunkCoordY, int sectorScale, int sectorStartU, int sectorStartV);
    int scale;

  private:
    virtual int GetCoordsToNeighbourTop(int x, int y, int scale) = 0;
    virtual int GetCoordsToNeighbourBottom(int x, int y, int scale) = 0; 
    virtual int GetCoordsToNeighbourLeft(int x, int y, int scale) = 0; 
    virtual int GetCoordsToNeighbourRight(int x, int y, int scale) = 0; 

    virtual int GetCoordsToNeighbourTopTopLeft(int x, int y, int scale) = 0;
    virtual int GetCoordsToNeighbourLeftTopLeft(int x, int y, int scale) = 0;

    virtual int GetCoordsToNeighbourTopTopRight(int x, int y, int scale) = 0;
    virtual int GetCoordsToNeighbourRightTopRight(int x, int y, int scale) = 0;

    virtual int GetCoordsToNeighbourBottomBottomRight(int x, int y, int scale) = 0; 
    virtual int GetCoordsToNeighbourRightBottomRight(int x, int y, int scale) = 0; 

    virtual int GetCoordsToNeighbourBottomBottomLeft(int x, int y, int scale) = 0; 
    virtual int GetCoordsToNeighbourLeftBottomLeft(int x, int y, int scale) = 0;

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
