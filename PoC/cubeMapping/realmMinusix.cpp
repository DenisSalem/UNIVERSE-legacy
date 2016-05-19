#include <iostream>
#include "realmMinusix.hpp"

RealmMinusix::RealmMinusix(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmMinusix::getCoordsToNeighbourTop(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusix::getCoordsToNeighbourBottom(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusix::getCoordsToNeighbourLeft(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusix::getCoordsToNeighbourRight(int x, int y, int scale) {
  return 0;
}
