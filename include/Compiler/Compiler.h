#pragma once
#include <string>
#include <vector>
#include "Vector.h"

class Compiler
{
public:
	Compiler(class Level* target);

	std::string ExportLevel(const std::string& filename);
	std::vector<v2> CompileGeometry();

private:

	Level* level;
};

