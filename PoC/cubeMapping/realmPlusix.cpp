#include <iostream>
#include "realmPlusix.hpp"

RealmPlusix::RealmPlusix(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmPlusix::getCoordsToNeighbourTop(int x, int y, int scale) {
  return scale + y + (scale - x - 1) * scale;
}

inline int RealmPlusix::getCoordsToNeighbourBottom(int x, int y, int scale) {
  return scale - y + scale - 1 + x *  scale;
}

inline int RealmPlusix::getCoordsToNeighbourLeft(int x, int y, int scale) {
  return scale + x + y * scale;
}

inline int RealmPlusix::getCoordsToNeighbourRight(int x, int y, int scale) {
  return scale - x + scale - 1 + (scale - y - 1) * scale;
}
