#include <iostream>
#include "realmPlusigrec.hpp"

RealmPlusigrec::RealmPlusigrec(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmPlusigrec::getCoordsToNeighbourTop(int x, int y, int scale) {
  return 0;
}

inline int RealmPlusigrec::getCoordsToNeighbourBottom(int x, int y, int scale) {
  return 0;
}

inline int RealmPlusigrec::getCoordsToNeighbourLeft(int x, int y, int scale) {
  return scale + x + y * scale;
}

inline int RealmPlusigrec::getCoordsToNeighbourRight(int x, int y, int scale) {
  return 0;
}
