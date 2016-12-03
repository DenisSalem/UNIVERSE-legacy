#include "cubeMap.hpp"
#include "planetFactory.hpp"
#include "renderPlanet.hpp"

class World {
  public:
    World(int LOD);
    CubeMap * cubeMap;
    RenderPlanet * renderPlanet;
    PlanetFactory * planetFactory;
};
