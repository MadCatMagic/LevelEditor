#include "Editor/Level.h"

// https://en.cppreference.com/w/cpp/container/unordered_map/unordered_map
namespace v2iHash
{
	std::size_t KeyHash::operator()(const v2i& k) const
	{
		return std::hash<int>()(k.x) ^
			(std::hash<int>()(k.y) << 1);
	}

	bool KeyEqual::operator()(const v2i& lhs, const v2i& rhs) const
	{
		return lhs.x == rhs.x && lhs.y == rhs.y;
	}
}

Level::Level(const v2i& dimensions)
{
	this->dimensions = dimensions;

	// q = (x + y - 1) / y for finding rounded up result of x/y
	for (int y = 0; y < (dimensions.y + TILE_CHUNK_SIZE - 1) / TILE_CHUNK_SIZE; y++)
		for (int x = 0; x < (dimensions.x + TILE_CHUNK_SIZE - 1) / TILE_CHUNK_SIZE; x++)
		{
			CreateChunk(v2i(x, y));
		}

	//tileData = new TileData[dimensions.x * dimensions.y];
}

Level::~Level()
{
	for each (TileChunk* c in chunks)
	{
		delete c;
	}
	chunks.clear();
	//delete[] tileData;
}

TileData* Level::GetTile(const v2i& pos)
{
	if (ValidPosition(pos))
	{
		int chunkIndex = chunkMap[v2i(pos.x / TILE_CHUNK_SIZE, pos.y / TILE_CHUNK_SIZE)];
		v2i correctPos = v2i(pos.x % TILE_CHUNK_SIZE, pos.y % TILE_CHUNK_SIZE);
		return &(chunks[chunkIndex]->tiles[correctPos.x][correctPos.y]);
	}
	return nullptr;
}

bool Level::ValidPosition(const v2i& pos) const
{
	return 0 <= pos.x && pos.x < dimensions.x && 0 <= pos.y && pos.y < dimensions.y;
}

void Level::CreateChunk(const v2i& chunkPos)
{
	TileChunk* c = new TileChunk();
	c->chunkPos = chunkPos;
	chunks.push_back(c);
	chunkMap.insert(std::make_pair(chunkPos, chunks.size() - 1));
}
