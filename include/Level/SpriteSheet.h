#pragma once
#include "Vector.h"
#include "LevelMaterial.h"

class SpriteSheetMaterial : public LevelMaterial
{
public:
	// this should be the location of the 
	SpriteSheetMaterial(const std::string& sheetFileDirectory);

	void LoadSprites(); // todo

private:
	void ReadFile();

	std::string directory;

};