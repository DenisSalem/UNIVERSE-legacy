#include <iostream>
#include "realmMinusigrec.hpp"

RealmMinusigrec::RealmMinusigrec(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmMinusigrec::getCoordsToNeighbourTop(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusigrec::getCoordsToNeighbourBottom(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusigrec::getCoordsToNeighbourLeft(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusigrec::getCoordsToNeighbourRight(int x, int y, int scale) {
  return 0;
}
