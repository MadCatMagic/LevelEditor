#include "Compiler/FileManager.h"
#include "Editor/Level.h"

#include <sstream>
#include <fstream>

#include <iostream>

void FileManager::ExportLevel(Level* level, const std::string& filename)
{
    Compiler compiler = Compiler(level);

    std::string data = compiler.ExportLevel(filename);
}

void FileManager::SaveLevel(Level* level, const std::string& filename)
{
    std::stringstream stream;

    for each (TileChunk* c in level->chunks)
    {
        stream << "c#" << std::to_string(c->chunkPos.x) << "," << std::to_string(c->chunkPos.y) << "#";

        for (int layer = 0; layer < TILE_CHUNK_LAYERS; layer++)
            for (int y = 0; y < TILE_CHUNK_SIZE; y++)
                for (int x = 0; x < TILE_CHUNK_SIZE; x++)
                {
                    TileData& t = c->tiles[layer][x][y];
                    stream << std::to_string(t.solid) << std::to_string(t.slant) << ",";
                }
    }

    std::ofstream writeStream = std::ofstream("levels/" + filename + ".lvl", std::ios::out | std::ios::trunc);
    std::string s = stream.str();
    writeStream.write((char*)s.data(), s.size());
    writeStream.close();
}

// the caller of this function takes ownership of the returned pointer
Level* FileManager::LoadLevel(const std::string& filename)
{
    std::ifstream stream("levels/" + filename + ".lvl");
    if (!stream.is_open())
    {
        std::cout << "pray for me: " << "levels/" + filename + ".lvl" << std::endl;
        return nullptr;
    }

    bool readingChunkPos = false;
    int tileIndex = 0;
    int layer = 0;
    int tileScanProgress = 0;

    std::string chunkPos;
    TileChunk* chunk = nullptr;

    Level* level = new Level(v2i::zero);

    v2i min;
    v2i max;

    char c;
    while (stream.get(c))
    {
        if (c == 'c')
            continue;
        if (!readingChunkPos && c == '#')
        {
            // first make sure to assign the previous chunk
            if (chunk != nullptr)
            {
                level->CreateChunk(chunk);
            }

            // then reset the values
            readingChunkPos = true;
            chunk = new TileChunk();
            chunkPos = "";
            tileIndex = 0;
            layer = 0;
        }
        else if (readingChunkPos)
        {
            if (c == '#')
            {
                // set the chunkPos of the current chunk
                readingChunkPos = false;
                int centre = 0;

                for (int i = 0; i < (int)chunkPos.size(); i++)
                    if (chunkPos[i] == ',')
                        centre = i;

                std::string part1 = chunkPos.substr(0, centre);
                std::string part2 = chunkPos.substr(centre + 1, chunkPos.size() - centre);
                v2i pos = v2i(std::stoi(part1), std::stoi(part2));
                chunk->chunkPos = pos;

                min = v2i(std::min(pos.x, min.x), std::min(pos.y, min.y));
                max = v2i(std::max(pos.x, max.x), std::max(pos.y, max.y));

                continue;
            }
            // otherwise just keep appending to the string
            chunkPos += c;
        }
        else if (chunk != nullptr)
        {
            if (c == ',')
            {
                // reset and increment index
                tileIndex++;
                if (tileIndex == TILE_CHUNK_SIZE * TILE_CHUNK_SIZE)
                {
                    tileIndex = 0;
                    layer++;
                }
                tileScanProgress = 0;
                
                continue;
            }

            // work out what part of the tile this corresponds to and coords of tile
            v2i pos = v2i(tileIndex % TILE_CHUNK_SIZE, tileIndex / TILE_CHUNK_SIZE);

            if (tileScanProgress == 0) // solid
                chunk->tiles[layer][pos.x][pos.y].solid = c == '1';

            else if (tileScanProgress == 1) // slant
                chunk->tiles[layer][pos.x][pos.y].slant = c - '0';

            tileScanProgress += 1;
        }
    }

    // at the end ensure to add the last chunk
    level->CreateChunk(chunk);
    level->dimensions = (max - min + v2i::one) * TILE_CHUNK_SIZE;

    stream.close();
    return level;
}
