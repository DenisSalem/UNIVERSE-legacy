#include <iostream>
#include "realmPlusix.hpp"

RealmPlusix::RealmPlusix(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmPlusix::getCoordsToNeighbourTop(int x, int y, int scale) {
  return 0;
}

inline int RealmPlusix::getCoordsToNeighbourBottom(int x, int y, int scale) {
  return 0;
}

inline int RealmPlusix::getCoordsToNeighbourLeft(int x, int y, int scale) {
  return 0;
}

inline int RealmPlusix::getCoordsToNeighbourRight(int x, int y, int scale) {
  return 0;
}
