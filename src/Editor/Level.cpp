#include "Editor/Level.h"

Level::Level(const v2i& chunkDimensions)
{
	this->dimensions = chunkDimensions * TILE_CHUNK_SIZE;

	for (int y = 0; y < chunkDimensions.y; y++)
		for (int x = 0; x < chunkDimensions.x; x++)
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

TileData* Level::GetTile(const v2i& pos, int layer)
{
	if (ValidPosition(pos))
	{
		int chunkIndex = chunkMap[v2i(pos.x / TILE_CHUNK_SIZE, pos.y / TILE_CHUNK_SIZE)];
		v2i correctPos = v2i(pos.x % TILE_CHUNK_SIZE, pos.y % TILE_CHUNK_SIZE);
		return &(chunks[chunkIndex]->tiles[layer][correctPos.x][correctPos.y]);
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
	CreateChunk(c);
}

void Level::CreateChunk(TileChunk* chunk)
{
	chunks.push_back(chunk);
	chunkMap.insert(std::make_pair(chunk->chunkPos, chunks.size() - 1));
}