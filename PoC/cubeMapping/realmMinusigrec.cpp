#include <iostream>
#include "realmMinusigrec.hpp"

RealmMinusigrec::RealmMinusigrec(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmMinusigrec::GetCoordsToNeighbourTop(int x, int y, int scale) {
  return x + (scale + y) * scale ;
}

inline int RealmMinusigrec::GetCoordsToNeighbourBottom(int x, int y, int scale) {
  return x + (y-scale) * scale;
}

inline int RealmMinusigrec::GetCoordsToNeighbourLeft(int x, int y, int scale) {
  return y - (x+1) * scale;
}

inline int RealmMinusigrec::GetCoordsToNeighbourRight(int x, int y, int scale) {
  return scale - 1 - y + (x - scale) * scale;
}

inline int RealmMinusigrec::GetCoordsToNeighbourTopTopLeft(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusigrec::GetCoordsToNeighbourLeftTopLeft(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusigrec::GetCoordsToNeighbourTopTopRight(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusigrec::GetCoordsToNeighbourRightTopRight(int x, int y, int scale) {
  return 0;
}

inline int RealmMinusigrec::GetCoordsToNeighbourBottomBottomLeft(int x, int y, int scale) {
  return scale + x + scale * (y - scale);
}

inline int RealmMinusigrec::GetCoordsToNeighbourLeftBottomLeft(int x, int y, int scale) {
  return scale + x + scale * (y - scale);
}

inline int RealmMinusigrec::GetCoordsToNeighbourBottomBottomRight(int x, int y, int scale) {
  return x - scale + (y - scale) * scale;
}

inline int RealmMinusigrec::GetCoordsToNeighbourRightBottomRight(int x, int y, int scale) {
  return x - scale + (y - scale) * scale;
}
