class Planet {
	public:
		Planet(brushes * HeightmapBrushes, int * seed);
		~Planet();
		
		void PushChunk(glm::vec3 id);
		void PopChunk(glm::vec3 id);
		void RenderDistances();
		FetchChunk(glm::vec3 id);

	private:
		int LookForFreeChunk();

		HeightmapBrushes * m_Brushes;

		Chunk m_ChunksPlusezed[64];
		Chunk m_ChunksPlusigrec[64];
		Chunk m_ChunksPlusix[64];
		Chunk m_ChunksMinusezed[64];
		Chunk m_ChunksMinusigrec[64];
		Chunk m_ChunksMinusix[64];
};
