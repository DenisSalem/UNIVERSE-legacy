#include <iostream>
#include "realmPlusezed.hpp"

RealmPlusezed::RealmPlusezed(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmPlusezed::getCoordsToNeighbourTop(int x, int y, int scale) {
  return (scale + y) * scale + x;
}

inline int RealmPlusezed::getCoordsToNeighbourBottom(int x, int y, int scale) {
  return (y - scale) * scale + x;
}

inline int RealmPlusezed::getCoordsToNeighbourLeft(int x, int y, int scale) {
  return y * scale + scale + x;
}

inline int RealmPlusezed::getCoordsToNeighbourRight(int x, int y, int scale) {
  return y * scale + x - scale;
}
