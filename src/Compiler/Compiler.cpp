#include "Compiler/Compiler.h"
#include "Editor/Level.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>

Compiler::Compiler(Level* target)
{
    level = target;
}

std::vector<ColliderSegment> Compiler::CompileGeometry()
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

            // if slanted always draw the slant
            if (t->slant != 0)
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

    // optimisation stage
    // want to combine straight lines into one and order the vertices of edges so i dont have to use pairs but just arrays with start points and end points

    // first put array into hash table to speed up lookup times
    std::unordered_map<v2, std::vector<int>, vecHash::KeyHash<v2, float>, vecHash::KeyEqual<v2>> pointMap;
    for (int i = 0; i < (int)pairs.size(); i++)
        pointMap[pairs[i]].push_back(i);
    
    std::vector<ColliderSegment> segments;

    std::unordered_set<v2, vecHash::KeyHash<v2, float>, vecHash::KeyEqual<v2>> seenPoints;
    for (int i = 0; i < (int)pairs.size(); i += 2)
    {
        // skip if either vertex has more than two connections
        if (pointMap[pairs[i]].size() != 2 || pointMap[pairs[i + 1]].size() != 2)
            continue;

        // skip if either vertex seen already
        if (seenPoints.find(pairs[i]) != seenPoints.end() || seenPoints.find(pairs[i + 1]) != seenPoints.end())
            continue;

        ColliderSegment segment;

        int forwards = i + 1;
        int backwards = i;

        seenPoints.insert(pairs[forwards]);
        seenPoints.insert(pairs[backwards]);

        // dangerous
        while (true)
        {
            if (segment.vec.size() != 0)
            {
                // only insert them if they are not already the first/last element
                bool firstSame = true;
                bool lastSame = true;
                if (segment.vec.back() != pairs[forwards])
                {
                    segment.vec.push_back(pairs[forwards]);
                    lastSame = false;
                }
                if (segment.vec[0] != pairs[backwards])
                {
                    segment.vec.insert(segment.vec.begin(), pairs[backwards]);
                    firstSame = false;
                }

                // if both flags are true then the cycle has ended, exit the loop
                if (firstSame && lastSame)
                    break;
            }
            else
            {
                // only for first iteration of while loop
                segment.vec.push_back(pairs[backwards]);
                segment.vec.push_back(pairs[forwards]);
            }

            if (pointMap[pairs[forwards]].size() == 2)
            {
                int index = (pointMap[pairs[forwards]][0] != forwards) ? pointMap[pairs[forwards]][0] : pointMap[pairs[forwards]][1];
                // want to use the other part of the pair the index corresponds to
                int newForwards = index - 2 * (index % 2) + 1;
                // only continue searching if exactly two points connect - corresponding to the two edges connected to this particular vertex
                if (pointMap[pairs[newForwards]].size() <= 2)
                    forwards = newForwards;
                // else found the end of the chain
            }

            // need to do two checks for circularity:
            // one after the first step in order to check for an odd-numbered edge loop
            // one after the second step to check for an even-numbered loop
            // check 1:
            if (pairs[forwards] == pairs[backwards])
            {
                segment.isLoop = true;
                segment.vec.push_back(pairs[forwards]);
                break;
            }

            // same for backwards
            if (pointMap[pairs[backwards]].size() == 2)
            {
                int index = (pointMap[pairs[backwards]][0] != backwards) ? pointMap[pairs[backwards]][0] : pointMap[pairs[backwards]][1];
                int newBackwards = index - 2 * (index % 2) + 1;
                if (pointMap[pairs[newBackwards]].size() <= 2)
                    backwards = newBackwards;
            }

            // check 2:
            if (pairs[forwards] == pairs[backwards])
            {
                segment.isLoop = true;
                segment.vec.push_back(pairs[forwards]);
                segment.vec.push_back(pairs[backwards]);
            }

            // emergency check
            if (seenPoints.find(pairs[forwards]) != seenPoints.end() && seenPoints.find(pairs[backwards]) != seenPoints.end())
                break;

            seenPoints.insert(pairs[backwards]);
            seenPoints.insert(pairs[forwards]);
        }
        segments.push_back(segment);
    }

    return segments;
}

std::string Compiler::ExportLevel(const std::string& filename)
{
    return std::string();
}
