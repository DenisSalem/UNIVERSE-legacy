#include <cstring>
#include "dataType.hpp"

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
      Heightmap m_NonOrientableBrush[HEIGHTMAP_BRUSHES_COUNT];
      Heightmap m_VoronoiBrush0[HEIGHTMAP_BRUSHES_COUNT];
      Heightmap m_VoronoiBrush1[HEIGHTMAP_BRUSHES_COUNT];
      Heightmap m_VoronoiBrush2[HEIGHTMAP_BRUSHES_COUNT];
      Heightmap m_PerlinBrush[HEIGHTMAP_BRUSHES_COUNT];
      Heightmap m_WarpBrush[HEIGHTMAP_BRUSHES_COUNT];
      Heightmap m_TurbulenceBrush[HEIGHTMAP_BRUSHES_COUNT];
      int * m_seed;
      int m_scale;
  };
}
