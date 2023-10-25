#include "Level/LevelMaterial.h"
#include "Level/Level.h"
#include "Level/SpriteSheet.h"

#include "Editor/Console.h"
#include <fstream>
#include <filesystem>
#include <sys/stat.h>

MaterialManager::~MaterialManager()
{
	for (size_t i = 0; i < materialList.size(); i++)
		if (materialList[i] != nullptr)
			delete materialList[i];
}

void MaterialManager::PopulateMaterialList()
{
	// try to find and load the pages.dat file:
	std::ifstream stream("res/materials/pages.dat");
	if (!stream.is_open())
	{
		Console::LogErr("MaterialManager could not find 'pages.dat' in res/materials. Panic!");
		return;
	}

	// each line contains sequential page names, which should correspond to folders 
	// containing the .mat files themselves that go in that particular page.
	std::string line;
	int lineNum = 0;
	while (std::getline(stream, line))
	{
		pages.push_back(PageData(line));
		std::string directory = "res/materials/" + line;

		// checks whether folder exists
		struct stat sb;
		if (stat(directory.c_str(), &sb))
		{
			Console::LogWarn("Material page " + line + " has no associated folder.Assuming the page is empty.");
			continue;
		}

		// load the specific materials in the corresponding folder.
		for (const auto& entry : std::filesystem::directory_iterator(directory))
			if (entry.path().extension() == ".mat")
			{
				LevelMaterial* mat = CreateMaterialFromFile(entry.path().string(), lineNum);
				if (mat != nullptr)
				{
					materialList[mat->id] = mat;
					pages[lineNum].length++;
				}
				else
					Console::LogErr("Something went wrong when trying to load the material file '" + entry.path().string() + "', uh oh!");
			}
		
		lineNum++;
	}

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
	if ((size_t)page < pages.size())
		return pages[page].length;
	return 0;
}

std::string MaterialManager::GetPageName(int page) const
{
	if ((size_t)page < pages.size())
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
		Console::LogErr("MaterialManager failed to open file: " + filepath);
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
			Console::LogErr(filepath + " on line " + std::to_string(lineNum) + " has no ':' seperator token");
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
				Console::LogErr(filepath + " on line " + std::to_string(lineNum) + " index is not in range, should be 0-15 inclusive, not '" + second + "'.");
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
				Console::LogErr(filepath + " on line " + std::to_string(lineNum) + " unknown material type '" + second + "'.");

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
			Console::LogErr(filepath + " on line " + std::to_string(lineNum) + " unknown identifier '" + first + "'.");
	}

	stream.close();

	// create material
	if (index == -1)
	{
		Console::LogWarn(filepath + " never set pageIndex, assuming to be the next available index.");
		index = pages[page].length;
	}
	int id = page * 16 + index;
	if (type == MaterialType::None || type == MaterialType::UNINITIALIZED)
	{
		if (type == MaterialType::UNINITIALIZED)
			Console::LogWarn(filepath + " did not provide a material type. Assuming type is none.");
		return (new LevelMaterial(editorColour, name))->Init(id);
	}
	else if (type == MaterialType::BasicTileMap)
	{
		SpriteSheetMaterial* mat = new SpriteSheetMaterial(editorColour, name);
		mat->LoadSprites(auxFilepath);
		return mat->Init(id);
	}

	// uh oh
	return nullptr;
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
	return (size_t)page < pages.size() && (id % 16) < pages[page].length;
}

LevelMaterial::LevelMaterial(const v3& editorCol, const std::string& materialName)
	: editorColour(editorCol), name(materialName)
{ }

LevelMaterial* LevelMaterial::Init(int materialId)
{
	id = materialId;
	return this;
}

MaterialManager::PageData::PageData(const std::string& pageName)
	: name(pageName)
{ }
