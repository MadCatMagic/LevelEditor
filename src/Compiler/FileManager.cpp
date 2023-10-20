#include "Compiler/FileManager.h"
#include "Level/Level.h"

#include "Level/Effects.h"

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
    // first dump the entity data
    for each (Entity* e in level->entities)
    {
        stream << e->GetType() << ";";
        stream << std::to_string(e->position.x) << "," << std::to_string(e->position.y) << "," << e->name << ",";
        stream << std::to_string(e->editorColour.x) << "," << std::to_string(e->editorColour.y) << ",";
        stream << std::to_string(e->editorColour.z) << "," << std::to_string(e->editorColour.w) << ",";
        for each (const std::string & segment in e->SaveData())
            stream << segment << ",";
        stream << ";";
    }
    stream << "~";

    // then areatrigger data
    for each (AreaTrigger* t in level->triggers)
    {
        stream << std::to_string(t->bottomLeft.x) << "," << std::to_string(t->bottomLeft.y) << ",";
        stream << std::to_string(t->topRight.x) << "," << std::to_string(t->topRight.y) << ",";
        stream << std::to_string(t->editorColour.x) << "," << std::to_string(t->editorColour.y) << ",";
        stream << std::to_string(t->editorColour.z) << "," << std::to_string(t->editorColour.w) << ",";
        stream << t->name << ";";
    }
    stream << "~";

    // actual level geometry comes afterwards
    for each (TileChunk* c in level->chunks)
    {
        stream << "c#" << std::to_string(c->chunkPos.x) << "," << std::to_string(c->chunkPos.y) << "#";

        for (int layer = 0; layer < TILE_CHUNK_LAYERS; layer++)
            for (int y = 0; y < TILE_CHUNK_SIZE; y++)
                for (int x = 0; x < TILE_CHUNK_SIZE; x++)
                {
                    TileData& t = c->tiles[layer][x][y];
                    stream << std::to_string(t.solid) << std::to_string(t.slant) << std::to_string(t.material) << ",";
                }
    }
    stream << "~";

    // finally we have the effect maps, 
    // signed by their name so that if the effect list order changes nothing unsavoury happens
    for (int i = 0; i < (int)EffectManager::instance->GetNumEffects(); i++)
    {
        Effect* effect = EffectManager::instance->GetEffect(i);
        stream << effect->name << "#";
        if (effect->perTile)
        {
            std::string data = effect->effectMap.tiles->GetData();
            stream << data.size() << "#";
            stream << data;
        }
        else 
        {
            // todo
        }
        stream << ";";
    }
    stream << "~";

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

    Level* level = new Level(v2i::zero);

    // entity data
    char c;
    // 0: pos.x, 1: pos.y, 2: name, then go until end
    int readingState = 0;

    std::string accumulate;
    std::vector<std::string> bitsNBobs;

    // load entity stuff
    Entity* e = nullptr;
    
    while (stream.get(c))
    {
        if (c == '~')
            break;
        else if (c == ';')
        {
            if (accumulate == "")
            {
                e->LoadData(bitsNBobs);
                level->entities.push_back(e);
                e = nullptr;
                readingState = 0;
            }
            else
                e = Entity::CreateEntityFromName(accumulate);
            accumulate = "";
        }
        else if (c == ',')
        {
            if (readingState == 0)
                e->position.x = std::stof(accumulate);
            else if (readingState == 1)
                e->position.y = std::stof(accumulate);
            else if (readingState == 2)
                e->name = accumulate;
            else if (readingState == 3)
                e->editorColour.x = std::stof(accumulate);
            else if (readingState == 4)
                e->editorColour.y = std::stof(accumulate);
            else if (readingState == 5)
                e->editorColour.z = std::stof(accumulate);
            else if (readingState == 6)
                e->editorColour.w = std::stof(accumulate);
            else
                bitsNBobs.push_back(accumulate);

            readingState++;
            accumulate = "";
        }
        else
            accumulate += c;
    }

    // load trigger stuff
    AreaTrigger* t = new AreaTrigger();
    readingState = 0;
    while (stream.get(c))
    {
        if (c == '~')
            break;
        else if (c == ';')
        {
            t->name = accumulate;
            level->triggers.push_back(t);
            t = new AreaTrigger();
            readingState = 0;
            accumulate = "";
        }
        else if (c == ',')
        {
            if (readingState == 0)
                t->bottomLeft.x = std::stoi(accumulate);
            else if (readingState == 1)
                t->bottomLeft.y = std::stoi(accumulate);
            else if (readingState == 2)
                t->topRight.x = std::stoi(accumulate);
            else if (readingState == 3)
                t->topRight.y = std::stoi(accumulate);
            else if (readingState == 4)
                t->editorColour.x = std::stof(accumulate);
            else if (readingState == 5)
                t->editorColour.y = std::stof(accumulate);
            else if (readingState == 6)
                t->editorColour.z = std::stof(accumulate);
            else if (readingState == 7)
                t->editorColour.w = std::stof(accumulate);

            readingState++;
            accumulate = "";
        }
        else
            accumulate += c;
    }
    delete t;
    
    // geometry data
    // scary
    bool readingChunkPos = false;
    int tileIndex = 0;
    int layer = 0;                          
    int tileScanProgress = 0;

    std::string chunkPos;
    TileChunk* chunk = nullptr;

    v2i min;
    v2i max;

    v2i pos;
    std::string materialBuffer;

    while (stream.get(c))
    {
        if (c == '~')
            break;
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
                chunk->tiles[layer][pos.x][pos.y].material = std::stoi(materialBuffer);
                materialBuffer = "";

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
            pos = v2i(tileIndex % TILE_CHUNK_SIZE, tileIndex / TILE_CHUNK_SIZE);

            if (tileScanProgress == 0) // solid
                chunk->tiles[layer][pos.x][pos.y].solid = c == '1';

            else if (tileScanProgress == 1) // slant
                chunk->tiles[layer][pos.x][pos.y].slant = c - '0';

            else if (tileScanProgress > 1)
                materialBuffer += c;

            tileScanProgress += 1;
        }
    }

    // at the end ensure to add the last chunk
    level->CreateChunk(chunk);
    level->dimensions = (max - min + v2i::one) * TILE_CHUNK_SIZE;

    // finally to do all the effect stuff
    bool readingEffectName = true;
    bool readingLength = false;
    std::string effectName;
    std::string effectData;

    size_t length = 0;
    int counter = 0;
    accumulate = "";

    // make sure to empty all the existing data
    EffectManager::instance->ReloadEffects();

    while (stream.get(c))
    {
        if (readingEffectName)
        {
            if (c == '~')
                break;
            else if (c == '#')
            {
                readingEffectName = false;
                readingLength = true;
            }
            else if (c == ';')
            {
                counter = 0;
                // effect data must be done reading, try to dispatch to effect
                Effect* e = EffectManager::instance->GetEffectFromName(effectName);
                // make sure the effect name actually refers to an effect
                if (e == nullptr)
                {
                    std::cout << "Error: effect name '" << effectName << "' appears to not exist and so cannot be loaded from file." << std::endl;
                    continue;
                }

                if (e->perTile)
                    e->effectMap.tiles->SetData(effectData);
                else
                {
                    // todo
                }
                effectData = "";
                effectName = "";
            }
            else
                effectName.push_back(c);
        }
        else if (readingLength)
        {
            if (c == '#')
            {
                length = std::stol(accumulate);
                readingLength = false;
                accumulate = "";
            }
            else
                accumulate.push_back(c);
        }
        if (!readingLength && !readingEffectName && counter < length)
        {
            if (counter > 0)
                effectData.push_back(c);
            counter++;
        }
        else if (!readingLength)
            readingEffectName = true;
    }

    stream.close();
    return level;
}
