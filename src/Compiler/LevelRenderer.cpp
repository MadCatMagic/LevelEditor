#include "Compiler/LevelRenderer.h"
#include "Level/Level.h"
#include "Level/LevelMaterial.h"

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
		LevelMaterial* mat = nullptr;
	};

	// precompute certain values that will be constant for each tile
	// with a small margin (+/- 2 tiles either side) around the camera bounds
	v2i bottomLeftBound = (v2i)(camera.position - camera.dimensions * 0.5f - v2(2));
	v2i topRightBound = (v2i)(camera.position + camera.dimensions * 0.5f + v2(2));
	std::vector<std::vector<TileRenderData>> renderData;
	for (int x = bottomLeftBound.x; x < topRightBound.x; x++)
	{
		std::vector<TileRenderData> columnData;
		for (int y = bottomLeftBound.y; y < topRightBound.y; y++)
		{
			TileRenderData t;

			const v2i tileOffsets[8]{
				v2i(1, 1),
				v2i(0, 1),
				v2i(-1, 1),
				v2i(-1, 0),
				v2i(-1, -1),
				v2i(0, -1),
				v2i(1, -1),
				v2i(1, 0)
			};

			for (int i = 0; i < 8; i++)
			{
				TileData* td = level->GetTile(v2i(x, y) + tileOffsets[i], 0);
				t.solidNeighbours += (td == nullptr || td->solid) << i;
			}

			TileData* td = level->GetTile(v2i(x, y), 0);
			if (td != nullptr)
			{
				t.mat = MaterialManager::currentInstance->GetMaterialFromId(td->material);
				t.solid = td->solid;
				t.slant = td->slant;
			}

			columnData.push_back(t);
		}
		renderData.push_back(columnData);
	}

	for (int x = 0; x < cam.pixelSize.x; x++)
		for (int y = 0; y < cam.pixelSize.y; y++)
		{
			v2 realPos = PixelToWorld(v2i(x, y));
			v2i lower = (v2i)realPos;
			v2i tileRenderDataPos = lower - bottomLeftBound;

			TileRenderData trd = renderData[tileRenderDataPos.x][tileRenderDataPos.y];

			if (trd.solid)
				data[tex->CoordToIndex(v2i(x, cam.pixelSize.y - y - 1))] = (v3)trd.mat->GetDataAtPoint(realPos) * (trd.solidNeighbours * (1.0f / 256.0f));

			/*
			TileData* t = level->GetTile(lower, 0);
			if (t != nullptr && t->solid)
			{
			}
			*/
		}

	tex->UpdateTexture();
	return tex;
}

v2i LevelRenderer::WorldToPixel(const v2& position) const
{
	return (v2i)v2::Scale(v2::Scale((position - cam.position), v2::Reciprocal(cam.dimensions)) + v2::one * 0.5f, cam.pixelSize);
}

v2 LevelRenderer::PixelToWorld(const v2i& position) const
{
	return v2::Scale(v2::Scale(position, v2::Reciprocal(cam.pixelSize)) - v2::one * 0.5f, cam.dimensions) + cam.position;
}
