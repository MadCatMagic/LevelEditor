#pragma once
#include "Vector.h"

struct TileData 
{
	TileData();

	bool solid;
	int slant;

};

class Level
{
public:
	Level(v2i dimensions);
	~Level();

	TileData* GetTile(v2i pos);

	v2i dimensions;

	bool ValidPosition(v2i pos) const;

private:
	TileData* tileData{ nullptr };

};

