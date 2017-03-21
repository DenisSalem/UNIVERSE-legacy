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
      
      Heightmap * GetNonOrientableBrush(int id);
      Heightmap * GetVoronoiBrush0(int id);
      Heightmap * GetVoronoiBrush1(int id);
      Heightmap * GetVoronoiBrush2(int id);
      Heightmap * GetPerlinBrush(int id);
      Heightmap * GetWarpBrush(int id);
      Heightmap * GetTurbulenceBrush(int id);
      
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
