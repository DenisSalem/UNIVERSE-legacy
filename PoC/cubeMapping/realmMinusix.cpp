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

inline int RealmMinusix::getCoordsToNeighbourTopLeft(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusix::getCoordsToNeighbourTopRight(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusix::getCoordsToNeighbourBottomLeft(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusix::getCoordsToNeighbourBottomRight(int x, int y, int scale) {
  return 0;
}
