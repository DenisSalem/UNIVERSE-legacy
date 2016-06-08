#include <iostream>
#include "realmPlusigrec.hpp"

RealmPlusigrec::RealmPlusigrec(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmPlusigrec::getCoordsToNeighbourTop(int x, int y, int scale) {
  return x + (scale + y) * scale;
}

inline int RealmPlusigrec::getCoordsToNeighbourBottom(int x, int y, int scale) {
  return x + (y-scale) * scale;
}

inline int RealmPlusigrec::getCoordsToNeighbourLeft(int x, int y, int scale) {
  return scale - y - 1 + (scale + x) * scale;
}

inline int RealmPlusigrec::getCoordsToNeighbourRight(int x, int y, int scale) {
  return y + (scale - x + scale - 1) * scale;
}
