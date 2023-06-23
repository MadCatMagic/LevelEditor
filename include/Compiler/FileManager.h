#pragma once
#include <string>
#include "Compiler/Compiler.h"

class Level;

class FileManager
{
public:
	void ExportLevel(Level* level, const std::string& filename);
	
	void SaveLevel(Level* level, const std::string& filename);
	Level* LoadLevel(const std::string& filename);
};

