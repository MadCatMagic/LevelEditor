#pragma once
#include "Vector.h"
#include "LevelMaterial.h"

struct SpriteSheetMaterial : public LevelMaterial
{
	// this should be the location of the actual .mat file relative to res/sprites/
	SpriteSheetMaterial(const std::string& sheetFileDirectory);
	~SpriteSheetMaterial();

	void LoadSprites(); // todo

private:
	void ReadFile();

	std::string directory;
	std::string spritesDirectory;

	class PixelTexture2D* texture = nullptr;
	class Texture2D* textureBase = nullptr;
};