#pragma once
#include <string>

class Compiler
{
public:
	Compiler(class Level* target);

	std::string ExportLevel(const std::string& filename);

private:
	std::string CompileGeometry();

	Level* level;
};

