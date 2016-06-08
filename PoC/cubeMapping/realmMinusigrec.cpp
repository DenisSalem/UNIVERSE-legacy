#include <iostream>
#include "realmMinusigrec.hpp"

RealmMinusigrec::RealmMinusigrec(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmMinusigrec::getCoordsToNeighbourTop(int x, int y, int scale) {
  return x + (scale + y) * scale ;
}

inline int RealmMinusigrec::getCoordsToNeighbourBottom(int x, int y, int scale) {
  return x + (y-scale) * scale;
}

inline int RealmMinusigrec::getCoordsToNeighbourLeft(int x, int y, int scale) {
  return y - (x+1) * scale;
}

inline int RealmMinusigrec::getCoordsToNeighbourRight(int x, int y, int scale) {
  return scale - 1 - y + (x - scale) * scale;
}
