#include <iostream>
#include "realmMinusix.hpp"

RealmMinusix::RealmMinusix(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmMinusix::GetCoordsToNeighbourTop(int x, int y, int scale) {
  return - y - 1 + x * scale;
}

inline int RealmMinusix::GetCoordsToNeighbourBottom(int x, int y, int scale) {
  return y - scale + (scale - 1 - x) * scale;
}

inline int RealmMinusix::GetCoordsToNeighbourLeft(int x, int y, int scale) {
  return - x - 1 + (scale - 1 - y) * scale;
}

inline int RealmMinusix::GetCoordsToNeighbourRight(int x, int y, int scale) {
  return x-scale + y * scale;
}

inline int RealmMinusix::GetCoordsToNeighbourTopTopLeft(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusix::GetCoordsToNeighbourLeftTopLeft(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusix::GetCoordsToNeighbourTopTopRight(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusix::GetCoordsToNeighbourRightTopRight(int x, int y, int scale) {
  return x - scale + scale * (scale + y);
}

inline int RealmMinusix::GetCoordsToNeighbourBottomBottomLeft(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusix::GetCoordsToNeighbourLeftBottomLeft(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusix::GetCoordsToNeighbourBottomBottomRight(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusix::GetCoordsToNeighbourRightBottomRight(int x, int y, int scale) {
  return x - scale + scale * (y - scale);
}
