#include "realm.hpp"

class RealmMinusix : public Realm {
  public:
    RealmMinusix(int LoD, float * min, float * max);

  private:
    int getCoordsToNeighbourTop(int x, int y);
    int getCoordsToNeighbourBottom(int x, int y);
    int getCoordsToNeighbourLeft(int x, int y);
    int getCoordsToNeighbourRight(int x, int y);
};
