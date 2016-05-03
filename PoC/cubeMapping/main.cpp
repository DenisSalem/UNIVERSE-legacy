#include <iostream>
#include "stampCollector.hpp"
#include "realmPlusezed.hpp"

int main(int argc, char ** argv) {
  StampCollector stampCollector = StampCollector(9);
  RealmPlusezed realmPlusezed = RealmPlusezed(9);
  realmPlusezed.AddStamp(stampCollector.GetCone());
  realmPlusezed.Noise(0,0,0);
  return 0; 
}
