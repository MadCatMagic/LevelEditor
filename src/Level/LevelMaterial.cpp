#include "Level/LevelMaterial.h"
#include "Level/Level.h"
#include "Level/SpriteSheet.h"

#include <iostream>
#include <fstream>

MaterialManager::~MaterialManager()
{
	for (int i = 0; i < materialList.size(); i++)
		if (materialList[i] != nullptr)
			delete materialList[i];
}

void MaterialManager::PopulateMaterialList()
{
	// page 1
	pages.push_back(PageData("Basic Page", 4));
	materialList[0] = CreateMaterialFromFile("res/materials/default.mat", 0);
	materialList[1] = CreateMaterialFromFile("res/materials/material1.mat", 0);
	materialList[2] = CreateMaterialFromFile("res/materials/material2.mat", 0);
	materialList[3] = CreateMaterialFromFile("res/materials/basic.mat", 0);

	// page 2
	pages.push_back(PageData("Second Page", 1));
	materialList[16] = CreateMaterialFromFile("res/materials/material3.mat", 0);

	currentInstance = this;
}

v3 MaterialManager::GetMaterialColour(int id)
{
	if (ValidMaterial(id))
		return materialList[id]->editorColour;
	return v3(1.0f, 1.0f, 1.0f);
}

std::string MaterialManager::GetMaterialName(int id)
{
	if (ValidMaterial(id))
		return materialList[id]->name;
	return "error_invalid_id";
}

int MaterialManager::GetPageLength(int page) const
{
	if (page < pages.size())
		return pages[page].length;
	return 0;
}

std::string MaterialManager::GetPageName(int page) const
{
	if (page < pages.size())
		return pages[page].name;
	return "error_invalid_page";
}

LevelMaterial* MaterialManager::CreateMaterialFromFile(const std::string& filepath, int page)
{
	// data which can be set
	MaterialType type = MaterialType::UNINITIALIZED;
	v4 editorColour;
	std::string name;
	std::string auxFilepath;
	int index = -1;

	std::ifstream stream(filepath);
	if (!stream.is_open())
	{
		std::cout << "SpriteSheetMaterial failed to open file: " << filepath << std::endl;
		return nullptr;
	}

	std::string line;
	int lineNum = 0;
	while (std::getline(stream, line))
	{
		lineNum++;

		// first ensure that there are no comments in the line and also remove trailing space
		size_t previousSpace = -1;
		for (size_t i = 0; i < line.size(); i++)
		{
			if (i < line.size() - 1 && line[i] == '/' && line[i + 1] == '/')
				break;
			if (line[i] != ' ')
				previousSpace = i;
		}
		line = line.substr(0, previousSpace + 1);
		if (line == "")
			continue;

		// seperate the string based of the ':' character which should be somewhere halfway
		size_t seperator = -1;
		for (size_t i = 0; i < line.size(); i++)
			if (line[i] == ':')
			{
				seperator = i;
				break;
			}
		if (seperator == -1)
		{
			std::cout << "Error: " << filepath << " on line " << lineNum << " has no ':' seperator token" << std::endl;
			return nullptr;
		}
		std::string first = line.substr(0, seperator);
		std::string second = line.substr(seperator + 1, line.size() - seperator);

		// simple the material name
		if (first == "name")
			name = second;

		// the index, should be 0 - 15 for that page
		else if (first == "pageIndex")
		{
			index = std::stoi(second);
			if (index > 15 || index < 0)
			{
				std::cout << "Error: " << filepath << " on line " << lineNum << " index is not in range, should be 0-15 inclusive, not '" << second << "'\n";
				index = 15;
			}
		}

		// matches to the MaterialType enum
		else if (first == "materialType")
			if (second == "basicTileMap")
				type = MaterialType::BasicTileMap;
			else if (second == "none")
				type = MaterialType::None;
			else
				std::cout << "Error: " << filepath << " on line " << lineNum << " unknown material type '" << second << "'\n";

		// where the corresponding actual spritemap is stored
		else if (first == "sprites")
			if (second == "$this")
				auxFilepath = filepath.substr(0, filepath.length() - 3) + "png";
			else
				auxFilepath = "res/materials/" + second;

		// the editorColour in the format r,g,b where they are float values
		else if (first == "editorColour")
		{
			// find the commas
			size_t f = -1;
			size_t s = -1;
			for (size_t i = 0; i < second.size(); i++)
				if (second[i] == ',')
					if (f == -1)
						f = i;
					else
					{
						s = i;
						break;
					}
			// interpret as colour values
			editorColour.x = std::stof(second.substr(0, f));
			editorColour.y = std::stof(second.substr(f + 1, s - f));
			editorColour.z = std::stof(second.substr(s + 1, second.size() - s));
		}

		// unrecognised identifier
		else
			std::cout << "Error: " << filepath << " on line " << lineNum << " unknown identifier '" << first << "'\n";
	}

	stream.close();

	// create material
	if (index == -1)
	{
		std::cout << "Warning: " << filepath << " never set pageIndex, assuming to be the next available index." << std::endl;
		index = pages[page].length;
	}
	int id = page * 16 + index;
	if (type == MaterialType::None || type == MaterialType::UNINITIALIZED)
	{
		if (type == MaterialType::UNINITIALIZED)
			std::cout << "Warning: " << filepath << " did not provide a material type. Assuming type is none." << std::endl;
		return (new LevelMaterial(editorColour, name))->Init(id);
	}
	else if (type == MaterialType::BasicTileMap)
	{
		SpriteSheetMaterial* mat = new SpriteSheetMaterial(editorColour, name);
		mat->LoadSprites(auxFilepath);
		return mat->Init(id);
	}
}

MaterialManager* MaterialManager::currentInstance = nullptr;

LevelMaterial* MaterialManager::GetMaterialFromId(int id) const
{
	if (ValidMaterial(id))
		return materialList[id];
	return nullptr;
}

bool MaterialManager::ValidMaterial(int id) const
{
	int page = id / 16;
	return page < pages.size() && (id % 16) < pages[page].length;
}

LevelMaterial::LevelMaterial(const v3& editorCol, const std::string& materialName)
	: editorColour(editorCol), name(materialName)
{ }

LevelMaterial* LevelMaterial::Init(int materialId)
{
	id = materialId;
	return this;
}

MaterialManager::PageData::PageData(const std::string& pageName, int pageLength)
	: name(pageName), length(pageLength)
{ }
