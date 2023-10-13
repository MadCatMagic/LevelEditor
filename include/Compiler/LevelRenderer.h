#pragma once
#include "Engine/PixelTexture2D.h"
#include "Level/Level.h"

struct TileRenderData
{
	bool solid = false;
	int slant = 0;
	// should be a bit mask so i can directly compare the value (0-127) with a sprite to load into that position.
	// gonna be a lot of work to write out all variations though...
	// 4  2  1
	// 8     128
	// 16 32 64
	int solidNeighbours = 0;
	struct LevelMaterial* mat = nullptr;
};

class LevelRenderer
{
public:
	LevelRenderer(Level* l);

	// the caller owns the return value
	PixelTexture2D* RenderCamera(Camera cam);

private:
	v2i WorldToPixel(const v2& position) const;
	v2 PixelToWorld(const v2i& position) const;

	Level* level;
	Camera cam{};
};