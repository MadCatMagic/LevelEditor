#pragma once
#include "Engine/PixelTexture2D.h"
#include "Level/Level.h"

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