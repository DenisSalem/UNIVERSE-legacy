#include <iostream>
#include "realmPlusigrec.hpp"

RealmPlusigrec::RealmPlusigrec(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmPlusigrec::GetCoordsToNeighbourTop(int x, int y, int scale) {
  return x + (scale + y) * scale;
}

inline int RealmPlusigrec::GetCoordsToNeighbourBottom(int x, int y, int scale) {
  return x + (y-scale) * scale;
}

inline int RealmPlusigrec::GetCoordsToNeighbourLeft(int x, int y, int scale) {
  return scale - y - 1 + (scale + x) * scale;
}

inline int RealmPlusigrec::GetCoordsToNeighbourRight(int x, int y, int scale) {
  return y + (scale - x + scale - 1) * scale;
}

inline int RealmPlusigrec::GetCoordsToNeighbourTopTopLeft(int x, int y, int scale) {
  return scale + x + scale * (scale + y);
}

inline int RealmPlusigrec::GetCoordsToNeighbourLeftTopLeft(int x, int y, int scale) {
  return 0;
}

inline int RealmPlusigrec::GetCoordsToNeighbourTopTopRight(int x, int y, int scale) {
  return x-scale + scale * ( scale + y);
}

inline int RealmPlusigrec::GetCoordsToNeighbourRightTopRight(int x, int y, int scale) {
  return 0;
}

inline int RealmPlusigrec::GetCoordsToNeighbourBottomBottomLeft(int x, int y, int scale) {
  return scale + x + scale * (y - scale);
}

inline int RealmPlusigrec::GetCoordsToNeighbourLeftBottomLeft(int x, int y, int scale) {
  return 0;
}

inline int RealmPlusigrec::GetCoordsToNeighbourBottomBottomRight(int x, int y, int scale) {
  return x - scale + scale * (y - scale);
}

inline int RealmPlusigrec::GetCoordsToNeighbourRightBottomRight(int x, int y, int scale) {
  return 0;
}
