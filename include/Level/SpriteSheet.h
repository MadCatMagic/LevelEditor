#pragma once
#include "Vector.h"
#include "LevelMaterial.h"

struct SpriteSheetMaterial : public LevelMaterial
{
	using LevelMaterial::LevelMaterial;
	~SpriteSheetMaterial();

	v4 GetDataAtPoint(const v2& worldPos, const TileRenderData& tile) override;

	void LoadSprites(const std::string& spritesDirectory);

private:
	class Image* texture = nullptr;
};