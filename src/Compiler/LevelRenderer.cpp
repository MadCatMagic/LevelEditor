#include "Compiler/LevelRenderer.h"
#include "Editor/Level.h"

LevelRenderer::LevelRenderer(Level* l)
	: level(l)
{
	
}

PixelTexture2D* LevelRenderer::RenderCamera(Camera camera)
{
	cam = camera;
	PixelTexture2D* tex = new PixelTexture2D();
	tex->CreateTexture(cam.pixelSize);
	v3* data = tex->GetRawData();

	for (int x = 0; x < cam.pixelSize.x; x++)
		for (int y = 0; y < cam.pixelSize.y; y++)
		{
			v2 realPos = PixelToWorld(v2i(x, y));
			v2i lower = (v2i)realPos;
			TileData* t = level->GetTile(lower, 0);
			if (t->solid)
				data[tex->CoordToIndex(v2i(x, y))] = v3(1.0f, 0.0f, 0.0f);
		}

	tex->UpdateTexture();
	return tex;
}

v2i LevelRenderer::WorldToPixel(const v2& position) const
{
	return (v2i)v2::Scale(v2::Scale((position - cam.centre), v2::Reciprocal(cam.dimensions)) + v2::one * 0.5f, cam.pixelSize);
}

v2 LevelRenderer::PixelToWorld(const v2i& position) const
{
	return v2::Scale(v2::Scale(position, v2::Reciprocal(cam.pixelSize)) - v2::one * 0.5f, cam.dimensions) + cam.centre;
}
