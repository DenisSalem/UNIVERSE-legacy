#include "world.hpp"

World::World(int LOD) {
  this->planetFactory = new PlanetFactory(LOD);
  this->renderPlanet = new RenderPlanet(
    this->planetFactory->indexSize,
    this->planetFactory->index,
    this->planetFactory->vertexSize,
    this->planetFactory->vertex
  );
}
