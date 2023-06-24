#pragma once
#include "Vector.h"
#include <vector>
#include <unordered_map>

struct TileData 
{
	bool solid = true;
	int slant = 0;
};

// 16 x 16
constexpr int TILE_CHUNK_SIZE = 16;
struct TileChunk
{
	TileData tiles[16][16]{};
	v2i chunkPos{};
};

namespace v2iHash
{
	struct KeyHash {
		std::size_t operator()(const v2i& k) const;
	};
	struct KeyEqual {
		bool operator()(const v2i& lhs, const v2i& rhs) const;
	};
}

class Level
{
public:
	friend class FileManager;
	friend class Compiler;

	Level(const v2i& chunkDimensions);
	~Level();

	TileData* GetTile(const v2i& pos);

	v2i dimensions;

	bool ValidPosition(const v2i& pos) const;

private:
	void CreateChunk(const v2i& chunkPos);
	void CreateChunk(TileChunk* chunk);

	std::vector<TileChunk*> chunks;
	std::unordered_map<v2i, int, v2iHash::KeyHash, v2iHash::KeyEqual> chunkMap;
};

