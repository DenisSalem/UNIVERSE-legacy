#include "realm.hpp"

class RealmMinusigrec : public Realm {
  public:
    RealmMinusigrec(int LoD, float * min, float * max);

  private:
    int GetCoordsToNeighbourTop(int x, int y, int scale);
    int GetCoordsToNeighbourBottom(int x, int y, int scale);
    int GetCoordsToNeighbourLeft(int x, int y, int scale);
    int GetCoordsToNeighbourRight(int x, int y, int scale);
    int GetCoordsToNeighbourTopTopLeft(int x, int y, int scale);
    int GetCoordsToNeighbourLeftTopLeft(int x, int y, int scale);
    int GetCoordsToNeighbourTopTopRight(int x, int y, int scale); 
    int GetCoordsToNeighbourRightTopRight(int x, int y, int scale); 
    int GetCoordsToNeighbourBottomBottomLeft(int x, int y, int scale); 
    int GetCoordsToNeighbourLeftBottomLeft(int x, int y, int scale); 
    int GetCoordsToNeighbourBottomBottomRight(int x, int y, int scale); 
    int GetCoordsToNeighbourRightBottomRight(int x, int y, int scale); 
};
