#pragma once
#include <string>
#include <vector>
#include "Vector.h"

struct ColliderSegment
{
	std::vector<v2> vec;
	bool isLoop = false;
};

class Compiler
{
public:
	Compiler(class Level* target);

	std::string ExportLevel(const std::string& filename);
	std::vector<ColliderSegment> CompileGeometry();

private:

	Level* level;
};

