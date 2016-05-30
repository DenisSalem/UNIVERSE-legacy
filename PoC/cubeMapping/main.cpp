#include <iostream>
#include "stampCollector.hpp"
#include "realmPlusezed.hpp"
#include "realmMinusix.hpp"
#include "png.hpp"

int main(int argc, char ** argv) {
  int LoD = 8;
  int scale = 2 << LoD;

  float min = 0;
  float max = 0;

  StampCollector stampCollector = StampCollector(LoD);

  RealmPlusezed realmPlusezed = RealmPlusezed(LoD, &min, &max);
  RealmMinusix realmMinusix = RealmMinusix(LoD, &min, &max);
  
  realmPlusezed.SetNeighbours(0,0,realmMinusix.GetRealm(),0);
  realmMinusix.SetNeighbours(0,0,0,realmPlusezed.GetRealm());

  realmPlusezed.AddStamp(stampCollector.GetCone());
  realmMinusix.AddStamp(stampCollector.GetCone());

  realmPlusezed.Noise(0,0,0);
  realmMinusix.Noise(0,0,0);

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
  float * realmMinusixFirstLayer = realmMinusix.GetRealm(0,0,0);

  for(int y = 0; y<scale4; y++) {
    for(int x = 0; x<scale3; x++) {
      if (x >= scale && x < scale2 && y >= scale && y < scale2) {
        color = (unsigned char) (255 * (realmPlusezedFirstLayer[ (x-scale) + scale * (y-scale)] - min) / (max - min));
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
