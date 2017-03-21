#ifndef INC_PROCEDURAL_ENGINE_HEIGHTMAP_BRUSHES

#define INC_PROCEDURAL_ENGINE_DATA_HEIGHTMAP_BRUSHES

#include <cstring>

#include "dataTypes.hpp"

#define HEIGHTMAP_SCALE 512
#define HEIGHTMAP_BRUSHES_COUNT 8

namespace ProceduralEngine {
  class HeightmapBrushes {
    public:
      HeightmapBrushes(int * seed);
      ~HeightmapBrushes();
      
      Heightmap * GetNonOrientableBrushes(int id);
      Heightmap * GetVoronoiBrushes0(int id);
      Heightmap * GetVoronoiBrushes1(int id);
      Heightmap * GetVoronoiBrushes2(int id);
      Heightmap * GetPerlinBrushes(int id);
      Heightmap * GetWarpBrushes(int id);
      Heightmap * GetTurbulenceBrushes(int id);
      
      int GetScale();

    private:
      Heightmap * m_NonOrientableBrushes;
      Heightmap * m_VoronoiBrushes0;
      Heightmap * m_VoronoiBrushes1;
      Heightmap * m_VoronoiBrushes2;
      Heightmap * m_PerlinBrushes;
      Heightmap * m_WarpBrushes;
      Heightmap * m_TurbulenceBrushes;
      int * m_Seed;
      int m_Scale;
  };
}

#endif
