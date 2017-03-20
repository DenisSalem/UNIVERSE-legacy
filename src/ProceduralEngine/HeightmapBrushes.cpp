#include "HeightmapBrushes.hpp"

namespace ProceduralEngine {
  HeightmapBrushes::HeightmapBrushes(int * seed) {
    m_Seed = seed;
    m_Scale = HEIGHTMAP_SCALE;
    memset( m_VoronoiBrush0, 0, sizeof(float) * HEIGHTMAP_BRUSHES_COUNT * HEIGHTMAP_AREA);
    memset( m_VoronoiBrush1, 0, sizeof(float) * HEIGHTMAP_BRUSHES_COUNT * HEIGHTMAP_AREA);
    memset( m_VoronoiBrush2, 0, sizeof(float) * HEIGHTMAP_BRUSHES_COUNT * HEIGHTMAP_AREA);
  }
}
