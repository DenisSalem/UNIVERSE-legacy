#include <iostream>
#include "realmPlusezed.hpp"

RealmPlusezed::RealmPlusezed(int LoD) : Realm(LoD) {
}

inline int RealmPlusezed::getCoordsToNeighbourTop(int x, int y) {
  return x*y;
}

inline int RealmPlusezed::getCoordsToNeighbourBottom(int x, int y) {
  return x*y;
}

inline int RealmPlusezed::getCoordsToNeighbourLeft(int x, int y) {
  return x*y;
}

inline int RealmPlusezed::getCoordsToNeighbourRight(int x, int y) {
  return x*y;
}
