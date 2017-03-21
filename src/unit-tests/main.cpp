#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "HeightmapBrushes.hpp" 
#include "dataTypes.hpp"

int main(int argc, char ** argv) {
  
  int seed[1024] = {0};
  ProceduralEngine::HeightmapBrushes brushes(seed);
  return 0;
}
