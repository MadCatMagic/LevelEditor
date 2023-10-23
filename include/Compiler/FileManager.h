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

private:
	const std::string latestVersion = "Version 1";
	int VersionNumFromString(const std::string& str);

	void MigrateToLatest(const std::string& filename, int fileVersion);
	std::string IncrementMigration(int& version, const std::string& data);
};

