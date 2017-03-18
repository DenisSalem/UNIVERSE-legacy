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
