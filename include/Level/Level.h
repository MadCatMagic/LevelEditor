#pragma once
#include "Vector.h"
#include <vector>
#include <unordered_map>

#include "Entity.h"
#include "AreaTrigger.h"

class Level;

typedef int TileMaterial;

struct TileData 
{
	bool solid = false;
	int slant = 0;
	TileMaterial material = 0;
};

// 16 x 16
constexpr int TILE_CHUNK_SIZE = 16;
constexpr int TILE_CHUNK_LAYERS = 4;
struct TileChunk
{
	TileChunk();
	// there are 4 layers
	// 0: playable space
	// 1: close background
	// 2: far background
	// 3: further background
	TileData tiles[TILE_CHUNK_LAYERS][TILE_CHUNK_SIZE][TILE_CHUNK_SIZE]{};
	v2i chunkPos{};
};

class Level
{
public:
	static Level* instance;

	friend class FileManager;
	friend class Compiler;
	friend struct Effect;

	Level(const v2i& chunkDimensions);
	~Level();

	TileData* GetTile(const v2i& pos, int layer);

	v2i dimensions;

	bool ValidPosition(const v2i& pos) const;

	// quite exposed
	std::vector<Entity*> entities;
	std::vector<AreaTrigger*> triggers;

	std::string file;

private:
	void CreateChunk(const v2i& chunkPos);
	void CreateChunk(TileChunk* chunk);

	std::vector<TileChunk*> chunks;
	std::unordered_map<v2i, int, vecHash::KeyHash<v2i, int>, vecHash::KeyEqual<v2i>> chunkMap;
};

