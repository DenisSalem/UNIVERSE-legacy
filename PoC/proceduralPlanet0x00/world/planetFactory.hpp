#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../common.hpp"
#include "cubeMap.hpp"

class PlanetFactory {
  public:
    PlanetFactory(int LOD, CubeMap * cubeMap);

    int cubeScale;
    int vertexSize;
    int indexSize;

    short int * index;
    
    CubeMap * cubeMap;

    glm::vec3 * vertex[6];
};
