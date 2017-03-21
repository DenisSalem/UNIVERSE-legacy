#include "HeightmapBrushes.hpp"

namespace ProceduralEngine {
  HeightmapBrushes::HeightmapBrushes(int * seed) {
    m_Seed = seed;
    m_Scale = HEIGHTMAP_SCALE;
    m_VoronoiBrushes0 = new Heightmap[HEIGHTMAP_BRUSHES_COUNT]();
    m_VoronoiBrushes1 = new Heightmap[HEIGHTMAP_BRUSHES_COUNT]();
    m_VoronoiBrushes2 = new Heightmap[HEIGHTMAP_BRUSHES_COUNT]();
    m_PerlinBrushes = new Heightmap[HEIGHTMAP_BRUSHES_COUNT]();
    m_WarpBrushes = new Heightmap[HEIGHTMAP_BRUSHES_COUNT]();
    m_TurbulenceBrushes = new Heightmap[HEIGHTMAP_BRUSHES_COUNT]();
  }

  HeightmapBrushes::~HeightmapBrushes() {
    delete[] m_VoronoiBrushes0;
    delete[] m_VoronoiBrushes1;
    delete[] m_VoronoiBrushes2;
    delete[] m_PerlinBrushes;
    delete[] m_WarpBrushes;
    delete[] m_TurbulenceBrushes;
  }
}
