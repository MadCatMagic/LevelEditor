#include "Compiler/Compiler.h"

Compiler::Compiler(Level* target)
{
    level = target;
}

std::string Compiler::CompileGeometry()
{
    return std::string();
}

std::string Compiler::ExportLevel(const std::string& filename)
{
    return std::string();
}
