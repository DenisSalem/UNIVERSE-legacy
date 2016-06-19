#include <iostream>
#include "stampCollector.hpp"
#include "realmPlusezed.hpp"
#include "realmMinusezed.hpp"
#include "realmMinusix.hpp"
#include "realmPlusix.hpp"
#include "realmPlusigrec.hpp"
#include "realmMinusigrec.hpp"
#include "png.hpp"

inline void setPixel(int index, float * chunk, PIXEL ** png, float min, float max, int x, int y) {
  unsigned char color = (unsigned char) (255 * (chunk[ index ] - min) / (max - min));
  png[x][y].Red	= color;
  png[x][y].Green = color;
  png[x][y].Blue = color;
}

int main(int argc, char ** argv) {
  int LoD = 7;
  int scale = 2 << (LoD+1);

  float min = 0;
  float max = 0;



  StampCollector stampCollector = StampCollector(LoD);

  // On peut envisager de créer une classe qui se chargera de faire toute ces initialisations.
  RealmPlusezed realmPlusezed = RealmPlusezed(LoD, &min, &max);
  RealmMinusezed realmMinusezed = RealmMinusezed(LoD, &min, &max);
  RealmMinusix realmMinusix = RealmMinusix(LoD, &min, &max);
  RealmPlusix realmPlusix = RealmPlusix(LoD, &min, &max);
  RealmPlusigrec realmPlusigrec = RealmPlusigrec(LoD, &min, &max);
  RealmMinusigrec realmMinusigrec = RealmMinusigrec(LoD, &min, &max);

  // Ajout du tampon de base
  // Avec ce système il est possible d'avoir des paysages très différents sur chaque royaume
  realmPlusezed.AddStamp(stampCollector.GetCone());
  realmMinusezed.AddStamp(stampCollector.GetCone());
  realmPlusix.AddStamp(stampCollector.GetCone());
  realmMinusix.AddStamp(stampCollector.GetCone());
  realmPlusigrec.AddStamp(stampCollector.GetCone());
  realmMinusigrec.AddStamp(stampCollector.GetCone());
  
  // Définition des royaumes voisins
  realmPlusezed.SetNeighbours(realmMinusigrec.GetRealm(),realmPlusigrec.GetRealm(),realmMinusix.GetRealm(),realmPlusix.GetRealm());
  realmMinusezed.SetNeighbours(realmPlusigrec.GetRealm(),realmMinusigrec.GetRealm(),realmMinusix.GetRealm(),realmPlusix.GetRealm());
  realmPlusix.SetNeighbours(realmMinusigrec.GetRealm(),realmPlusigrec.GetRealm(),realmPlusezed.GetRealm(),realmMinusezed.GetRealm());
  realmMinusix.SetNeighbours(realmMinusigrec.GetRealm(),realmPlusigrec.GetRealm(),realmMinusezed.GetRealm(),realmPlusezed.GetRealm());
  realmPlusigrec.SetNeighbours(realmPlusezed.GetRealm(),realmMinusezed.GetRealm(),realmMinusix.GetRealm(),realmPlusix.GetRealm());
  realmMinusigrec.SetNeighbours(realmMinusezed.GetRealm(),realmPlusezed.GetRealm(),realmMinusix.GetRealm(),realmPlusix.GetRealm());
  
  // On lance la fonction récursive de bruit sur chaque royaume
  //realmPlusezed.Noise(0,0,0);
  //realmMinusezed.Noise(0,0,0);
  //realmPlusix.Noise(0,0,0);
  //realmMinusix.Noise(0,0,0);
  //realmPlusigrec.Noise(0,0,0);
  //realmMinusigrec.Noise(0,0,0);

  // On test sur des layers supérieurs
  

  realmMinusezed.AllocateChunk(1,0,0);
  realmMinusezed.AllocateChunk(1,1,0);
  realmMinusezed.AllocateChunk(1,0,1);
  realmMinusezed.AllocateChunk(1,1,1);

  realmPlusezed.AllocateChunk(1,0,0);
  realmPlusezed.AllocateChunk(1,1,0);
  realmPlusezed.AllocateChunk(1,0,1);
  realmPlusezed.AllocateChunk(1,1,1);

  realmMinusigrec.AllocateChunk(1,0,0);
  realmMinusigrec.AllocateChunk(1,1,0);
  realmMinusigrec.AllocateChunk(1,0,1);
  realmMinusigrec.AllocateChunk(1,1,1);

  realmPlusigrec.AllocateChunk(1,0,0);
  realmPlusigrec.AllocateChunk(1,1,0);
  realmPlusigrec.AllocateChunk(1,0,1);
  realmPlusigrec.AllocateChunk(1,1,1);

  realmMinusix.AllocateChunk(1,0,0);
  realmMinusix.AllocateChunk(1,1,0);
  realmMinusix.AllocateChunk(1,0,1);
  realmMinusix.AllocateChunk(1,1,1);

  realmPlusix.AllocateChunk(1,0,0);
  realmPlusix.AllocateChunk(1,1,0);
  realmPlusix.AllocateChunk(1,0,1);
  realmPlusix.AllocateChunk(1,1,1);

  realmPlusezed.Noise(1,0,0);
  realmPlusezed.Noise(1,1,0);
  realmPlusezed.Noise(1,0,1);
  realmPlusezed.Noise(1,1,1);

  //realmMinusigrec.Noise(1,0,0);
  //realmMinusigrec.Noise(1,1,0);
  //realmMinusigrec.Noise(1,0,1);
  //realmMinusigrec.Noise(1,1,1);

  //realmMinusix.Noise(1,0,0);
  //realmMinusix.Noise(1,1,0);
  //realmMinusix.Noise(1,0,1);
  //realmMinusix.Noise(1,1,1);
  // A partir de là les six heightmaps de bases sont terminées, 
  // y a plus qu'a envoyer tout ça dans un png pour le plaisir de vos yeux.

  PIXEL ** png = new PIXEL*[scale * 3];
  for (int x =0; x < scale * 3 ; x++) {
    png[x] = new PIXEL[scale * 4];
  }

  int scale4 = scale * 4;
  int scale3 = scale * 3;
  int scale2 = scale * 2;
  int halfScale = scale >> 1;
  unsigned char color;

  float * chunk00 = realmPlusezed.GetRealm(1,0,0);
  float * chunk01 = realmPlusezed.GetRealm(1,1,0);
  float * chunk02 = realmPlusezed.GetRealm(1,0,1);
  float * chunk03 = realmPlusezed.GetRealm(1,1,1);

  float * chunk40 = realmMinusezed.GetRealm(1,0,0);
  float * chunk41 = realmMinusezed.GetRealm(1,1,0);
  float * chunk42 = realmMinusezed.GetRealm(1,0,1);
  float * chunk43 = realmMinusezed.GetRealm(1,1,1);

  float * chunk10 = realmMinusigrec.GetRealm(1,0,0);
  float * chunk11 = realmMinusigrec.GetRealm(1,1,0);
  float * chunk12 = realmMinusigrec.GetRealm(1,0,1);
  float * chunk13 = realmMinusigrec.GetRealm(1,1,1);

  float * chunk30 = realmPlusigrec.GetRealm(1,0,0);
  float * chunk31 = realmPlusigrec.GetRealm(1,1,0);
  float * chunk32 = realmPlusigrec.GetRealm(1,0,1);
  float * chunk33 = realmPlusigrec.GetRealm(1,1,1);

  float * chunk20 = realmMinusix.GetRealm(1,0,0);
  float * chunk21 = realmMinusix.GetRealm(1,1,0);
  float * chunk22 = realmMinusix.GetRealm(1,0,1);
  float * chunk23 = realmMinusix.GetRealm(1,1,1);

  float * chunk50 = realmPlusix.GetRealm(1,0,0);
  float * chunk51 = realmPlusix.GetRealm(1,1,0);
  float * chunk52 = realmPlusix.GetRealm(1,0,1);
  float * chunk53 = realmPlusix.GetRealm(1,1,1);

  for(int y = 0; y<scale4; y++) {
    for(int x = 0; x<scale3; x++) {
      // Plusix top left
      if (x >= scale2 && x < scale2 + halfScale && y >= scale && y < scale + halfScale) {
        setPixel(x - scale2 + halfScale * (y-scale), chunk50, png, min, max, x, y);
      }
      // Plusix top right
      else if (x >= scale2 + halfScale && x < scale3 + halfScale && y >= scale && y < scale + halfScale) {
        setPixel(x - scale2 - halfScale + halfScale * (y-scale), chunk51, png, min, max, x, y);
      }
      // Plusix bottom right
      else if (x >= scale2 + halfScale && x < scale3 + halfScale && y >= scale + halfScale && y < scale2) {
        setPixel(x - scale2 - halfScale + halfScale * (y-scale-halfScale), chunk53, png, min, max, x, y);
      }
      // Plusix bottom left
      else if (x >= scale2 && x < scale3 + halfScale && y >= scale + halfScale && y < scale2) {
        setPixel(x - scale2 + halfScale * (y-scale-halfScale), chunk52, png, min, max, x, y);
      }
      // Minusezed top left
      else if (x >= scale && x < scale + halfScale && y >= scale3 && y < scale3 + halfScale) {
        setPixel(x - scale + halfScale * (y-scale3), chunk40, png, min, max, x, y);
      }
      // Minusezed top right
      else if (x >= scale + halfScale && x < scale2 && y >= scale3 && y < scale3 + halfScale) {
        setPixel(x - scale - halfScale + halfScale * (y-scale3), chunk41, png, min, max, x, y);
      }
      // Minusezed bottom right
      else if (x >= scale + halfScale && x < scale2 && y >= scale3 + halfScale && y < scale4) {
        setPixel(x - scale - halfScale + halfScale * (y-scale3-halfScale), chunk43, png, min, max, x, y);
      }
      // Minusezed bottom left
      else if (x >= scale && x < scale2 && y >= scale3 + halfScale && y < scale4) {
        setPixel(x - scale + halfScale * (y-scale3-halfScale), chunk42, png, min, max, x, y);
      }
      // Plusigrec top left
      else if (x >= scale && x < scale + halfScale && y >= scale2 && y < scale2 + halfScale) {
        setPixel(x - scale + halfScale * (y-scale2), chunk30, png, min, max, x, y);
      }
      // Plusigrec top right
      else if (x >= scale + halfScale && x < scale2 && y >= scale2 && y < scale2 + halfScale) {
        setPixel(x - scale - halfScale + halfScale * (y-scale2), chunk31, png, min, max, x, y);
      }
      // Plusigrec bottom right
      else if (x >= scale + halfScale && x < scale2 && y >= scale2 + halfScale && y < scale3) {
        setPixel(x - scale - halfScale + halfScale * (y-scale2 - halfScale), chunk33, png, min, max, x, y);
      }
      // Plusigrec bottom left
      else if (x >= scale && x < scale + halfScale && y >= scale2 + halfScale && y < scale3) {
        setPixel(x - scale * (y - scale2 - halfScale), chunk32, png, min, max, x, y);
      }
      // Minusix top left
      else if (x >= 0 && x < halfScale && y >= scale && y < scale + halfScale) {
        setPixel(x + halfScale * (y-scale), chunk20, png, min, max, x, y);
      }
      // Minusix top right
      else if (x >= halfScale && x < scale && y >= scale && y < scale + halfScale) {
        setPixel(x - halfScale + halfScale * (y-scale), chunk21, png, min, max, x, y);
       }
      // Minusix bottom right
      else if (x >= halfScale && x < scale && y >= scale + halfScale && y < scale2) {
        setPixel(x - halfScale + halfScale * (y-scale-halfScale), chunk23, png, min, max, x, y);
      }
      // Minusix bottom left
      else if (x >= 0 && x < halfScale && y >= scale + halfScale && y < scale2) {
        setPixel(x + halfScale * (y-scale-halfScale), chunk22, png, min, max, x, y);
      }
      // Minusigrec top left
      else if (x >= scale && x < scale + halfScale && y >=  0 && y < halfScale) {
        setPixel((x-scale) + halfScale * y, chunk10, png, min, max, x, y);
      }
      // Minusigrec top right
      else if (x >= scale+halfScale && x < scale2 && y >=  0 && y < halfScale) {
        setPixel((x-scale-halfScale) + halfScale * y, chunk11, png, min, max, x, y);
      }
      // Minusigrec bottom right
      else if (x >= scale+halfScale && x < scale2 && y >=  halfScale && y < scale) {
        setPixel((x-scale-halfScale) + halfScale * (y-halfScale), chunk13, png, min, max, x, y);
      }
      // Minusigrec bottom left
      else if (x >= scale && x < scale+halfScale && y >=  halfScale && y < scale) {
        setPixel((x-scale) + halfScale * (y-halfScale), chunk12, png, min, max, x, y);
      }
      // Plusezed top left
      else if (x >= scale && x < scale + halfScale && y >= scale && y < scale + halfScale) {
        setPixel((x-scale) + halfScale * (y-scale), chunk00, png, min, max, x, y);
      }
      // Plusezed top right
      else if (x >= scale+halfScale && x < scale2 && y >= scale && y < scale + halfScale) {
        setPixel((x-scale-halfScale) + halfScale * (y-scale), chunk01, png, min, max, x, y);
      }
      // Plusezed bottom right
      else if (x >= scale+halfScale && x < scale2 && y >= scale+halfScale && y < scale2) {
        setPixel(x -scale-halfScale + halfScale * (y - scale-halfScale), chunk03, png, min, max, x, y);
      }
      // Plusezed bottom left
      else if (x >= scale && x < scale+halfScale && y >= scale+halfScale && y < scale2) {
        setPixel((x-scale) + halfScale * (y-scale-halfScale), chunk02, png, min, max, x, y);
      }
      else {
	png[x][y].Red	= 0;
	png[x][y].Green = 255;
	png[x][y].Blue = 0;
      }
    }
  }

  writePng(png,1536,2048);

  return 0; 
}
