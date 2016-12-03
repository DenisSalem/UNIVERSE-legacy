#include "vertexFactory.hpp"
#include "renderSphere.hpp"

class Sphere {
  public:
    Sphere(int LOD);
    RenderSphere * renderSphere;
    VertexFactory * vertexFactory;
};
