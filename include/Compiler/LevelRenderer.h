#pragma once
#include "Engine/PixelTexture2D.h"

struct Camera
{
	v2i centre;
	v2 dimensions = v2::one;
	v2i pixelSize = v2i::one * 16;
};

class LevelRenderer
{
public:
	LevelRenderer(class Level* l);

	// the caller owns the return value
	PixelTexture2D* RenderCamera(Camera cam);

private:
	v2i WorldToPixel(const v2& position) const;
	v2 PixelToWorld(const v2i& position) const;

	Level* level;
	Camera cam{};
};