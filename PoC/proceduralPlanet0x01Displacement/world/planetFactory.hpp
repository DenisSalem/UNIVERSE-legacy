#ifndef _PLANET_FACTORY_
  #define _PLANET_FACTORY_

  #include <glm/glm.hpp>
  #include <glm/gtx/transform.hpp>
  #include <glm/gtc/type_ptr.hpp>
  #include "../common.hpp"
  #include "cubeMap.hpp"

  class PlanetFactory {
    public:
      PlanetFactory(int LOD, CubeMap * cubeMap);
      int getCubeScale();
      int getVertexSize();
      int getIndexSize();

      short int * index;
    
      CubeMap * cubeMap;

      glm::vec3 * vertex[6];

    private:
      int cubeScale;
      int vertexSize;
      int indexSize;

  };
#endif
