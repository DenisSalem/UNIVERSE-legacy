#ifndef INC_PROCEDURAL_ENGINE_DATA_TYPE

#define INC_PROCEDURAL_ENGINE_DATA_TYPE

#define HEIGHTMAP_AREA 262144

namespace ProceduralEngine {
  typedef float Heightmap[HEIGHTMAP_AREA];
  
  typedef struct Chunk {
    Heightmap heightmap;
    Chunk * ne;
    Chunk * nw;
    Chunk * se;
    Chunk * sw;
    bool allocated;
    float centerX;
    float centerY;
    float halfScale;
  } Chunk;
}

#endif
