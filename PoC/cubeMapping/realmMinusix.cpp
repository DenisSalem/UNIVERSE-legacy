#include <iostream>
#include "realmMinusix.hpp"

RealmMinusix::RealmMinusix(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmMinusix::getCoordsToNeighbourTop(int x, int y, int scale) {
  return - y - 1 + x * scale;
}

inline int RealmMinusix::getCoordsToNeighbourBottom(int x, int y, int scale) {
  return y - scale + (scale - 1 - x) * scale;
}

inline int RealmMinusix::getCoordsToNeighbourLeft(int x, int y, int scale) {
  return - x - 1 + (scale - 1 - y) * scale;
}

inline int RealmMinusix::getCoordsToNeighbourRight(int x, int y, int scale) {
  return x-scale + y * scale;
}
