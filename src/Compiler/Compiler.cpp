#include "Compiler/Compiler.h"
#include "Editor/Level.h"

#include <vector>

Compiler::Compiler(Level* target)
{
    level = target;
}

std::vector<v2> Compiler::CompileGeometry()
{
    std::vector<v2> pairs;

    for (int y = 0; y < level->dimensions.y; y++)
        for (int x = 0; x < level->dimensions.x; x++)
        {
            // work out what collision this particular tile should have
            v2i pos = v2i(x, y);
            TileData* t = level->GetTile(pos);
            if (t == nullptr)
                continue;

            // only solid tiles contribute to collision to avoid getting messy
            if (!t->solid)
                continue;

            // work out which directions should have geometry
            const v2i directions[4]{ v2i(1, 0), v2i(0, 1), v2i(-1, 0), v2i(0, -1) };
            const bool defaultSlopeAirDirections[4]{ true, true, false, false }; // from the perspective of the direction

            bool air[4]{false, false, false, false}; // initialized to false by default
            for (int i = 0; i < 4; i++)
            {
                TileData* neighbour = level->GetTile(pos + directions[i]);
                if (neighbour == nullptr)
                    continue;

                if (!neighbour->solid)
                    air[i] = true;
                else if (neighbour->slant == 0)
                    continue;

                // checks whether the slope has an air gap towards this tile
                else
                    air[i] = defaultSlopeAirDirections[(neighbour->slant - 1 - i + 4) % 4]; // the +4 ensures it is never negative
            }

            // if all surrounding tiles are solid and not sloped then skip
            if (!(air[0] || air[1] || air[2] || air[3]))
                continue;

            // work out what geometry you should have
            // solid square
            const v2 solidPairs[5]{
                    v2(1.0f, 0.0f), v2(1.0f, 1.0f),
                    v2(0.0f, 1.0f),
                    v2(0.0f, 0.0f),
                    v2(1.0f, 0.0f)
            };
            if (t->slant == 0)
            {
                for (int i = 0; i < 4; i++)
                    if (air[i])
                    {
                        pairs.push_back(solidPairs[i] + pos);
                        pairs.push_back(solidPairs[i + 1] + pos);
                    }
            }
            // slanted
            else
            {
                if (t->slant == 1 || t->slant == 3)
                {
                    pairs.push_back(v2(0.0f, 0.0f) + pos);
                    pairs.push_back(v2(1.0f, 1.0f) + pos);
                }
                else
                {
                    pairs.push_back(v2(1.0f, 0.0f) + pos);
                    pairs.push_back(v2(0.0f, 1.0f) + pos);
                }
                for (int i = 0; i < 4; i++)
                {
                    if (air[i] && !defaultSlopeAirDirections[(t->slant + 1 - i + 4) % 4]) // the +4 ensures it is never negative
                    {
                        pairs.push_back(solidPairs[i] + pos);
                        pairs.push_back(solidPairs[i + 1] + pos);
                    }
                }
            }
        }
    return  pairs;
}

std::string Compiler::ExportLevel(const std::string& filename)
{
    return std::string();
}
