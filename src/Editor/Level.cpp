#include "Editor/Level.h"

TileData::TileData()
	: solid(true), slant(0)
{
	
}

Level::Level(v2i dimensions)
{
	this->dimensions = dimensions;
	tileData = new TileData[dimensions.x * dimensions.y];
}

Level::~Level()
{
	delete[] tileData;
}

TileData* Level::GetTile(v2i pos)
{
	if (ValidPosition(pos))
		return &tileData[pos.y * dimensions.x + pos.x];
	return nullptr;
}

bool Level::ValidPosition(v2i pos) const
{
	return 0 <= pos.x && pos.x < dimensions.x && 0 <= pos.y && pos.y < dimensions.y;
}