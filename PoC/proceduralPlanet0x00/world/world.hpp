#include "planetFactory.hpp"
#include "renderPlanet.hpp"

class World {
  public:
    World(int LOD);
    RenderPlanet * renderPlanet;
    PlanetFactory * planetFactory;
};
