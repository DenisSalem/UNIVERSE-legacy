#include <iostream>
#include "realmMinusezed.hpp"

RealmMinusezed::RealmMinusezed(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmMinusezed::GetCoordsToNeighbourTop(int x, int y, int scale) {
  return (scale + y) * scale + x;
}

inline int RealmMinusezed::GetCoordsToNeighbourBottom(int x, int y, int scale) {
  return (y - scale) * scale + x;
}

inline int RealmMinusezed::GetCoordsToNeighbourLeft(int x, int y, int scale) {
  return - x - 1 + (scale - 1 - y) * scale;
}

inline int RealmMinusezed::GetCoordsToNeighbourRight(int x, int y, int scale) {
  return scale - x + scale - 1 + (scale - 1 - y) * scale;
}

inline int RealmMinusezed::GetCoordsToNeighbourTopTopLeft(int x, int y, int scale) {
  return scale + x + scale * (scale + y);
}

inline int RealmMinusezed::GetCoordsToNeighbourLeftTopLeft(int x, int y, int scale) {
  return - x - 1 + scale * ( - y - 1) ;
}

inline int RealmMinusezed::GetCoordsToNeighbourTopTopRight(int x, int y, int scale) {
  return x - scale + scale * ( scale + y);
}

inline int RealmMinusezed::GetCoordsToNeighbourRightTopRight(int x, int y, int scale) {
  return scale - x -1 + scale * ( - y);
}

inline int RealmMinusezed::GetCoordsToNeighbourBottomBottomLeft(int x, int y, int scale) {
  return x - scale + scale * ( y - scale + 2);
}

inline int RealmMinusezed::GetCoordsToNeighbourLeftBottomLeft(int x, int y, int scale) {
  return -x + scale * (scale - y + scale - 1);
}

inline int RealmMinusezed::GetCoordsToNeighbourBottomBottomRight(int x, int y, int scale) {
  return x - scale + scale * ( y - scale);
}

inline int RealmMinusezed::GetCoordsToNeighbourRightBottomRight(int x, int y, int scale) {
  return scale - x -1 + scale * ( scale - y + scale );
}
