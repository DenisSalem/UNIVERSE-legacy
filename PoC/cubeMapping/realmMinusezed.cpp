#include <iostream>
#include "realmMinusezed.hpp"

RealmMinusezed::RealmMinusezed(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmMinusezed::getCoordsToNeighbourTop(int x, int y, int scale) {
  return (scale + y) * scale + x;
}

inline int RealmMinusezed::getCoordsToNeighbourBottom(int x, int y, int scale) {
  return (y - scale) * scale + x;
}

inline int RealmMinusezed::getCoordsToNeighbourLeft(int x, int y, int scale) {
  return - x - 1 + (scale - 1 - y) * scale;
}

inline int RealmMinusezed::getCoordsToNeighbourRight(int x, int y, int scale) {
  return scale - x + scale - 1 + (scale - 1 - y) * scale;
}
