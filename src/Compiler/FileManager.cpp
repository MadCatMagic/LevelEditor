#include "Compiler/FileManager.h"

#include <sstream>

void FileManager::ExportLevel(Level* level, const std::string& filename)
{
    Compiler compiler = Compiler(level);

    std::string data = compiler.ExportLevel("levels/" + filename);
}

void FileManager::SaveLevel(Level* level, const std::string& filename)
{
    std::stringstream stream;

}

// the caller of this function takes ownership of the returned pointer
Level* FileManager::LoadLevel(const std::string& filename)
{
    return nullptr;
}
