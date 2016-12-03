#include <iostream>
#include "realmPlusix.hpp"

RealmPlusix::RealmPlusix(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmPlusix::GetCoordsToNeighbourTop(int x, int y, int scale) {
  return scale + y + (scale - x - 1) * scale;
}

inline int RealmPlusix::GetCoordsToNeighbourBottom(int x, int y, int scale) {
  return scale - y + scale - 1 + x *  scale;
}

inline int RealmPlusix::GetCoordsToNeighbourLeft(int x, int y, int scale) {
  return scale + x + y * scale;
}

inline int RealmPlusix::GetCoordsToNeighbourRight(int x, int y, int scale) {
  return scale - x + scale - 1 + (scale - y - 1) * scale;
}

inline int RealmPlusix::GetCoordsToNeighbourTopTopLeft(int x, int y, int scale) {
  return scale + y + scale * (- x - 1);
}

inline int RealmPlusix::GetCoordsToNeighbourLeftTopLeft(int x, int y, int scale) {
  return scale + x + scale * (scale + y);
}

inline int RealmPlusix::GetCoordsToNeighbourTopTopRight(int x, int y, int scale) {
  return scale + y + scale * (scale - x + scale - 1);
}

inline int RealmPlusix::GetCoordsToNeighbourRightTopRight(int x, int y, int scale) {
  return scale - x + scale - 1 + scale * ( scale - y - scale -1);
}

inline int RealmPlusix::GetCoordsToNeighbourBottomBottomLeft(int x, int y, int scale) {
  return scale - y + scale - 1 + scale * (scale + x);
}

inline int RealmPlusix::GetCoordsToNeighbourLeftBottomLeft(int x, int y, int scale) {
  return scale + x + scale * ( y - scale);
}

inline int RealmPlusix::GetCoordsToNeighbourBottomBottomRight(int x, int y, int scale) {
  return scale - y + scale - 1 + scale * (x-scale);
}

inline int RealmPlusix::GetCoordsToNeighbourRightBottomRight(int x, int y, int scale) {
  return scale - x + scale - 1 + scale * (scale - y + scale - 1);
}
