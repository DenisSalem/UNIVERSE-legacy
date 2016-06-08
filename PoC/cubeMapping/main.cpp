#include <iostream>
#include "stampCollector.hpp"
#include "realmPlusezed.hpp"
#include "realmMinusezed.hpp"
#include "realmMinusix.hpp"
#include "realmPlusix.hpp"
#include "realmPlusigrec.hpp"
#include "realmMinusigrec.hpp"
#include "png.hpp"

int main(int argc, char ** argv) {
  int LoD = 8;
  int scale = 2 << LoD;

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
  realmPlusezed.Noise(0,0,0);
  realmMinusezed.Noise(0,0,0);
  realmPlusix.Noise(0,0,0);
  realmMinusix.Noise(0,0,0);
  realmPlusigrec.Noise(0,0,0);
  realmMinusigrec.Noise(0,0,0);

  // A partir de là les six heightmaps de bases sont terminées, 
  // y a plus qu'a envoyer tout ça dans un png pour le plaisir de vos yeux.

  PIXEL ** png = new PIXEL*[scale * 3];
  for (int x =0; x < scale * 3 ; x++) {
    png[x] = new PIXEL[scale * 4];
  }

  int scale4 = scale * 4;
  int scale3 = scale * 3;
  int scale2 = scale * 2;
  unsigned char color;

  float * realmPlusezedFirstLayer = realmPlusezed.GetRealm(0,0,0);
  float * realmMinusezedFirstLayer = realmMinusezed.GetRealm(0,0,0);
  float * realmMinusixFirstLayer = realmMinusix.GetRealm(0,0,0);
  float * realmPlusixFirstLayer = realmPlusix.GetRealm(0,0,0);
  float * realmPlusigrecFirstLayer = realmPlusigrec.GetRealm(0,0,0);
  float * realmMinusigrecFirstLayer = realmMinusigrec.GetRealm(0,0,0);

  for(int y = 0; y<scale4; y++) {
    for(int x = 0; x<scale3; x++) {
      if (x >= scale && x < scale2 && y >= scale && y < scale2) {
        color = (unsigned char) (255 * (realmPlusezedFirstLayer[ (x-scale) + scale * (y-scale)] - min) / (max - min));
	png[x][y].Red	= color;
	png[x][y].Green = color;
	png[x][y].Blue = color;
      }
      else if (x >= scale && x < scale2 && y >= scale3 && y < scale4) {
        color = (unsigned char) (255 * (realmMinusezedFirstLayer[ (x-scale) + scale * (y-scale3)] - min) / (max - min));
	png[x][y].Red	= color;
	png[x][y].Green = color;
	png[x][y].Blue = color;
      }
      else if (x >= 0 && x < scale && y >= scale && y < scale2) { // minuxX
        color = (unsigned char) (255 * (realmMinusixFirstLayer[ x + scale*(y-scale)] - min) / (max - min));
        png[x][y].Red = color; 
        png[x][y].Green	= color;
        png[x][y].Blue = color;
      }
      else if (x >= scale2 && x < scale3 && y >= scale && y < scale2 ) {
        color = (unsigned char) (255 * (realmPlusixFirstLayer[ (x - scale2) + scale*(y-scale)] - min) / (max - min));
        png[x][y].Red = color; 
        png[x][y].Green	= color;
        png[x][y].Blue = color;
      }
      else if (x >= scale && x < scale2 && y >= scale2 && y < scale3 ) {
        color = (unsigned char) (255 * (realmPlusigrecFirstLayer[ (x - scale) + scale*(y-scale2)] - min) / (max - min));
        png[x][y].Red = color; 
        png[x][y].Green	= color;
        png[x][y].Blue = color;
      }
      else if (x >= scale && x < scale2 && y >= 0 && y < scale ) {
        color = (unsigned char) (255 * (realmMinusigrecFirstLayer[ (x - scale) + scale*y] - min) / (max - min));
        png[x][y].Red = color; 
        png[x][y].Green	= color;
        png[x][y].Blue = color;
      }
      else {
	png[x][y].Red	= 0;
	png[x][y].Green = 255;
	png[x][y].Blue = 0;
      }
    }
  }

  writePng(png,scale);

  return 0; 
}
