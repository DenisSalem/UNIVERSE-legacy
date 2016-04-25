#include "world.hpp"

World::World(int LOD) {
  this->cubeMap = new CubeMap(LOD);
  this->planetFactory = new PlanetFactory(LOD, this->cubeMap);
  this->renderPlanet = new RenderPlanet(
    this->planetFactory
  );
}
