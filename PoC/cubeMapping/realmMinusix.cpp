#include <iostream>
#include "realmMinusix.hpp"

RealmMinusix::RealmMinusix(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmMinusix::getCoordsToNeighbourTop(int x, int y) {
  return x*y;
}

inline int RealmMinusix::getCoordsToNeighbourBottom(int x, int y) {
  return x*y;
}

inline int RealmMinusix::getCoordsToNeighbourLeft(int x, int y) {
  return x*y;
}

inline int RealmMinusix::getCoordsToNeighbourRight(int x, int y) {
  return x*y;
}
