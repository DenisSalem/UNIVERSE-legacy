#include <iostream>
#include "realmPlusezed.hpp"

RealmPlusezed::RealmPlusezed(int LoD, float * min, float * max) : Realm(LoD, min, max) {
}

inline int RealmPlusezed::GetCoordsToNeighbourTop(int x, int y, int scale) {
  return (scale + y) * scale + x;
}

inline int RealmPlusezed::GetCoordsToNeighbourBottom(int x, int y, int scale) {
  return (y - scale) * scale + x;
}

inline int RealmPlusezed::GetCoordsToNeighbourLeft(int x, int y, int scale) {
  return y * scale + scale + x;
}

inline int RealmPlusezed::GetCoordsToNeighbourRight(int x, int y, int scale) {
  return y * scale + x - scale;
}

inline int RealmPlusezed::GetCoordsToNeighbourTopTopLeft(int x, int y, int scale) {
  return scale + x + scale * (scale + y);
}

inline int RealmPlusezed::GetCoordsToNeighbourLeftTopLeft(int x, int y, int scale) {
  return scale + x + scale * ( scale + y);
}

inline int RealmPlusezed::GetCoordsToNeighbourTopTopRight(int x, int y, int scale) {
  return x - scale + scale * (scale + y);
}

inline int RealmPlusezed::GetCoordsToNeighbourRightTopRight(int x, int y, int scale) {
  return x - scale + scale * ( scale + y);
}

inline int RealmPlusezed::GetCoordsToNeighbourBottomBottomLeft(int x, int y, int scale) {
  return scale + x + scale * ( y - scale );
}

inline int RealmPlusezed::GetCoordsToNeighbourLeftBottomLeft(int x, int y, int scale) {
  return scale + x + scale * ( y - scale);
}

inline int RealmPlusezed::GetCoordsToNeighbourBottomBottomRight(int x, int y, int scale) {
  return x-scale + scale * ( y - scale);
}

inline int RealmPlusezed::GetCoordsToNeighbourRightBottomRight(int x, int y, int scale) {
  return x - scale + scale * ( y - scale);
}
