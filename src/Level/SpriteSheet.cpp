#include "Level/SpriteSheet.h"
#include "Engine/Texture/Image.h"
#include "Compiler/LevelRenderer.h"

SpriteSheetMaterial::~SpriteSheetMaterial()
{
	if (texture != nullptr)
		delete texture;
}

v4 SpriteSheetMaterial::GetDataAtPoint(const v2& worldPos, const TileRenderData& tile)
{
	int xOffset = (int)(worldPos.x * 16.0f) % 16 + tile.slant * 16;
	int yOffset = (int)(worldPos.y * 16.0f) % 16;
	return texture->At(v2i(xOffset, yOffset));
}

void SpriteSheetMaterial::LoadSprites(const std::string& spritesDirectory)
{
	texture = new Image(spritesDirectory);
	texture->LoadData();
}
