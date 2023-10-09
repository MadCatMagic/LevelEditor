#include "Level/SpriteSheet.h"

#include <fstream>
#include <iostream>

SpriteSheetMaterial::SpriteSheetMaterial(const std::string& sheetFileDirectory)
	: LevelMaterial(v3(), "")
{
	directory = "res/sprites/" + sheetFileDirectory;
	ReadFile();
}

void SpriteSheetMaterial::ReadFile()
{
	std::ifstream stream(directory);
	if (!stream.is_open())
	{
		std::cout << "SpriteSheetMaterial failed to open file: " << directory << std::endl;
		return;
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
			std::cout << "Error: " << directory << " on line " << lineNum << " has no ':' seperator token" << std::endl;
			return;
		}
		std::string first = line.substr(0, seperator);
		std::string second = line.substr(seperator + 1, line.size() - seperator);

		// simple the material name
		if (first == "name")
			name = second;

		// matches to the MaterialType enum
		else if (first == "materialType")
			if (second == "basicTileMap")
				type = MaterialType::BasicTileMap;
			else
				std::cout << "Error: " << directory << " on line " << lineNum << " unknown material type '" << second << "'\n";

		// where the corresponding actual spritemap is stored
		else if (first == "sprites")
			if (second == "$this")
				spritesDirectory = directory.substr(0, directory.length() - 3) + "png";
			else
				spritesDirectory = "res/sprites/" + second;

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
			std::cout << "Error: " << directory << " on line " << lineNum << " unknown identifier '" << first << "'\n";
	}

	stream.close();
}

void SpriteSheetMaterial::LoadSprites()
{
}
