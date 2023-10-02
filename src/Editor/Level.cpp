#include "Editor/Level.h"

#include "imgui.h"
#include "Editor/EditorGizmos.h"

Level::Level(const v2i& chunkDimensions)
{
	this->dimensions = chunkDimensions * TILE_CHUNK_SIZE;

	for (int y = 0; y < chunkDimensions.y; y++)
		for (int x = 0; x < chunkDimensions.x; x++)
		{
			CreateChunk(v2i(x, y));
		}

	//tileData = new TileData[dimensions.x * dimensions.y];
}

Level::~Level()
{
	for each (TileChunk* c in chunks)
	{
		delete c;
	}
	chunks.clear();
	//delete[] tileData;
}

TileData* Level::GetTile(const v2i& pos, int layer)
{
	if (ValidPosition(pos))
	{
		int chunkIndex = chunkMap[v2i(pos.x / TILE_CHUNK_SIZE, pos.y / TILE_CHUNK_SIZE)];
		v2i correctPos = v2i(pos.x % TILE_CHUNK_SIZE, pos.y % TILE_CHUNK_SIZE);
		return &(chunks[chunkIndex]->tiles[layer][correctPos.x][correctPos.y]);
	}
	return nullptr;
}

bool Level::ValidPosition(const v2i& pos) const
{
	return 0 <= pos.x && pos.x < dimensions.x && 0 <= pos.y && pos.y < dimensions.y;
}

void Level::CreateChunk(const v2i& chunkPos)
{
	TileChunk* c = new TileChunk();
	c->chunkPos = chunkPos;
	CreateChunk(c);
}

void Level::CreateChunk(TileChunk* chunk)
{
	chunks.push_back(chunk);
	chunkMap.insert(std::make_pair(chunk->chunkPos, chunks.size() - 1));
}

TileChunk::TileChunk()
{
	for (int x = 0; x < TILE_CHUNK_SIZE; x++)
		for (int y = 0; y < TILE_CHUNK_SIZE; y++)
		{
			tiles[0][x][y].solid = true;
		}
}

void Camera::UI()
{
	ImGui::InputFloat2("Dimensions", &dimensions.x);
	ImGui::InputInt2("Pixel Size", &pixelSize.x);
}

void Camera::Gizmos()
{
	EditorGizmos::SetColour(editorColour);
	EditorGizmos::DrawLine(position - dimensions * 0.5f, position + dimensions * 0.5f, 2.0f);
	EditorGizmos::DrawLine(position - v2(dimensions.x, -dimensions.y) * 0.5f, position + v2(dimensions.x, -dimensions.y) * 0.5f, 2.0f);
	EditorGizmos::DrawRectOutline(position - dimensions * 0.5f, position + dimensions * 0.5f, 2.0f);
}

std::vector<std::string> Camera::SaveData() const
{
	std::vector<std::string> o;
	o.push_back(std::to_string(dimensions.x));
	o.push_back(std::to_string(dimensions.y));
	o.push_back(std::to_string(pixelSize.x));
	o.push_back(std::to_string(pixelSize.y));
	return o;
}

void Camera::LoadData(std::vector<std::string> data)
{
	dimensions.x = std::stof(data[0]);
	dimensions.y = std::stof(data[1]);
	pixelSize.x = std::stof(data[2]);
	pixelSize.y = std::stof(data[3]);
}

Entity* Entity::CreateEntityFromName(const std::string& name)
{
	if (name == "Entity")
		return new Entity();
	if (name == "Camera")
		return new Camera();
	return nullptr;
}
