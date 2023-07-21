#pragma once
#include <string>
#include <vector>
#include <unordered_map>
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
	std::vector<ColliderSegment> CompileGeometry(int layer);

private:
	std::vector<v2> pairs;
	std::unordered_map<v2, std::vector<int>, vecHash::KeyHash<v2, float>, vecHash::KeyEqual<v2>> pointMap;
	std::unordered_map<v2, int, vecHash::KeyHash<v2, float>, vecHash::KeyEqual<v2>> seenPoints;

	int StepThroughGeometry(int startingIndex, bool rhsIsSolid);

	Level* level;
};

