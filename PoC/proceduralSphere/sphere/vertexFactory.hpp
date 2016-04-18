#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../common.hpp"

class VertexFactory {
  public:
    VertexFactory(int LOD);

    int cubeScale;
    int vertexSize;
    int indexSize;

    short int * index[2];
    
    glm::vec3 * vertex[6];
};
