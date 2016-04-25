#include "sphere.hpp"

Sphere::Sphere(int LOD) {
  this->vertexFactory = new VertexFactory(LOD);
  this->renderSphere = new RenderSphere(
    this->vertexFactory->indexSize,
    this->vertexFactory->index,
    this->vertexFactory->vertexSize,
    this->vertexFactory->vertex
  );
}
