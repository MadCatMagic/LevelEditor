#include "Compiler/Compiler.h"
#include "Editor/Level.h"

#include <unordered_set>

Compiler::Compiler(Level* target)
{
    level = target;
}

std::vector<ColliderSegment> Compiler::CompileGeometry(int layer)
{
    pairs.clear();

    for (int y = 0; y < level->dimensions.y; y++)
        for (int x = 0; x < level->dimensions.x; x++)
        {
            // work out what collision this particular tile should have
            v2i pos = v2i(x, y);
            TileData* t = level->GetTile(pos, layer);
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
                TileData* neighbour = level->GetTile(pos + directions[i], layer);
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

            // for all of these the solid side is on the left hand side going forward the vectors
            const v2 diagonalPairs[8]{
                v2(1.0f, 1.0f), v2(0.0f, 0.0f),
                v2(0.0f, 1.0f), v2(1.0f, 0.0f),
                v2(0.0f, 0.0f), v2(1.0f, 1.0f),
                v2(1.0f, 0.0f), v2(0.0f, 1.0f)
            };

            // if slanted always draw the slant
            if (t->slant != 0)
            {
                pairs.push_back(diagonalPairs[(t->slant - 1) * 2] + pos);
                pairs.push_back(diagonalPairs[(t->slant - 1) * 2 + 1] + pos);
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
    pointMap.clear();
    for (int i = 0; i < (int)pairs.size(); i++)
        pointMap[pairs[i]].push_back(i);
    

    // this section combines segments so that it is easier to compute optimisations and to ensure that each segment is a continuous line
    std::vector<ColliderSegment> segments;
    seenPoints.clear();
    for (int i = 0; i < (int)pairs.size(); i += 2)
    {
        // skip if both vertices have more than two connections
        //if (pointMap[pairs[i]].size() != 2 && pointMap[pairs[i + 1]].size() != 2)
        //    continue;

        // skip if both vertices already satisfied
        if (seenPoints[pairs[i]] >= (int)pointMap[pairs[i]].size() / 2 && seenPoints[pairs[i + 1]] >= (int)pointMap[pairs[i + 1]].size() / 2)
            continue;

        // expensive but necessary?
        bool skip = false;
        for each (const ColliderSegment & segment in segments)
        {
            for (int j = 0; j < (int)segment.vec.size() - 1; j++)
            {
                if ((pairs[i] == segment.vec[j] &&
                    pairs[i + 1] == segment.vec[j + 1]) ||
                    (pairs[i] == segment.vec[j + 1] &&
                        pairs[i + 1] == segment.vec[j]))
                    skip = true;
            }
        }
        if (skip)
            continue;

        ColliderSegment segment;

        int forwards = i + 1;
        int backwards = i;

        if (seenPoints[pairs[backwards]] < (int)pointMap[pairs[backwards]].size() / 2)
            seenPoints[pairs[backwards]] += 1;
        if (seenPoints[pairs[forwards]] < (int)pointMap[pairs[forwards]].size() / 2)
            seenPoints[pairs[forwards]] += 1;

        // if forward has this condition then backward wont and vice versa
        const bool forwardsIsRhsSolid = false;

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
                    seenPoints[pairs[forwards]] += 1;
                    lastSame = false;
                }
                if (segment.vec[0] != pairs[backwards])
                {
                    segment.vec.insert(segment.vec.begin(), pairs[backwards]);
                    seenPoints[pairs[backwards]] += 1;
                    firstSame = false;
                }

                // if both flags are true then the cycle has ended, exit the loop
                if (firstSame && lastSame)
                    break;

                // emergency check
                if (seenPoints[pairs[forwards]] == pointMap[pairs[forwards]].size() / 2 + 1 && seenPoints[pairs[backwards]] == pointMap[pairs[backwards]].size() / 2 + 1)
                    break;
            }
            else
            {
                // only for first iteration of while loop
                segment.vec.push_back(pairs[backwards]);
                segment.vec.push_back(pairs[forwards]);
            }

            // replaced by function
            int prevForwards = forwards;
            forwards = StepThroughGeometry(forwards, forwardsIsRhsSolid);

            // need to do two checks for circularity:
            // one after the first step in order to check for an odd-numbered edge loop
            // one after the second step to check for an even-numbered loop
            // check 1:
            if ((forwards / 2) == (backwards / 2) && forwards != prevForwards) // only do check if forwards changed
            {
                segment.isLoop = true;
                segment.vec.push_back(pairs[forwards]);
                break;
            }

            // same for backwards
            backwards = StepThroughGeometry(backwards, !forwardsIsRhsSolid);

            // check 2:
            if ((forwards / 2) == (backwards / 2))
            {
                segment.isLoop = true;
                segment.vec.push_back(pairs[forwards]);
                segment.vec.push_back(pairs[backwards]);
            }
        }
        segments.push_back(segment);
    }

    // now want to work out what to do with all the vertices with more than 2 connections
    // perhaps try and get it to be a solvable eulerian graph (all connected with all vertices having even degree) and then solve that?
    return segments;
}

#define PI_CONST 3.1415926535897932384626433832795f
int Compiler::StepThroughGeometry(int startingIndex, bool rhsIsSolid)
{
    int forwards = startingIndex;
    int numConnections = pointMap[pairs[forwards]].size();
    std::pair<v2, int> dirArray[8]{}; // can never be more than 8 outgoing edges

    if (numConnections == 1)
        return startingIndex;

    int thisPoint = 0;
    for (int i = 0; i < numConnections; i++)
    {
        int newIndex = pointMap[pairs[forwards]][i];
        if (newIndex == forwards)
            thisPoint = i;

        // want to use the other part of the pair the index corresponds to
        int correspondingIndex = newIndex - 2 * (newIndex % 2) + 1;
        // work out the vector coming out of this point
        dirArray[i].first = v2::Normalize(pairs[correspondingIndex] - pairs[forwards]);
        dirArray[i].second = correspondingIndex;
    }

    // optimised case for only 2 connections
    // also fallback just in case
    if (numConnections == 2)
    {
        auto newForwards = dirArray[1 - thisPoint];
        return newForwards.second;
    }

    // general case
    int newForwards = 0;
    float smallestTheta = 8.0f; // result of acosf in range (0, pi)
    for (int i = 0; i < numConnections; i++)
    {
        if (i == thisPoint)
            continue;

        // dot product = cos theta, theta = angle between vectors
        v2 a = dirArray[thisPoint].first;
        v2 b = dirArray[i].first;

        // first make sure the target isnt actually just already full
        //if (seenPoints[a] >= pointMap[a].size())
        //    continue;

        float theta = acosf(v2::Dot(a, b));
        // cross product to find whether the angle is > 180
        float zcross = a.x * b.y - b.x * a.y;
        if (zcross < 0.0f)
            theta = 2.0f * PI_CONST - theta;

        // invert
        // rhsIsSolid corresponds to the starting vector
        if (rhsIsSolid)
            theta = 2.0f * PI_CONST - theta;

        // compare with others
        if (theta < smallestTheta)
        {
            newForwards = dirArray[i].second;
            smallestTheta = theta;
        }
    }

    return newForwards;
}

std::string Compiler::ExportLevel(const std::string& filename)
{
    return std::string();
}
