#pragma once
#include "Vector.h"
#include <vector>
#include <unordered_map>

typedef int TileMaterial;

struct TileData 
{
	bool solid = false;
	int slant = 0;
	TileMaterial material = 0;
};

// 16 x 16
constexpr int TILE_CHUNK_SIZE = 16;
constexpr int TILE_CHUNK_LAYERS = 4;
struct TileChunk
{
	TileChunk();
	// there are 4 layers
	// 0: playable space
	// 1: close background
	// 2: far background
	// 3: further background
	TileData tiles[TILE_CHUNK_LAYERS][TILE_CHUNK_SIZE][TILE_CHUNK_SIZE]{};
	v2i chunkPos{};
};

struct Entity
{
	v2 position;
	std::string name;

	v4 editorColour = v4(1.0f, 0.0f, 0.0f, 1.0f);

	inline virtual void UI() { };
	inline virtual void Gizmos() { };

	virtual inline std::string GetType() const { return "Entity"; }
	virtual inline bool PlaceAtCentreOfTile() const { return true; }

	virtual inline std::vector<std::string> SaveData() const { return std::vector<std::string>(); }
	virtual inline void LoadData(std::vector<std::string> data) { }

	static Entity* CreateEntityFromName(const std::string& name);
};

struct Camera : Entity
{
	v2 dimensions = v2::one;
	v2i pixelSize = v2i::one * 16;

	void UI() override;
	void Gizmos() override;

	inline std::string GetType() const override { return "Camera"; }
	inline bool PlaceAtCentreOfTile() const { return false; }

	std::vector<std::string> SaveData() const;
	void LoadData(std::vector<std::string> data);
};

struct AreaTrigger
{
	v2i bottomLeft;
	v2i topRight;

	int groupId = -1;
	std::string name;

	v4 editorColour = v4(1.0f, 0.0f, 1.0f, 0.6f);
};

class Level
{
public:
	friend class FileManager;
	friend class Compiler;

	Level(const v2i& chunkDimensions);
	~Level();

	TileData* GetTile(const v2i& pos, int layer);

	v2i dimensions;

	bool ValidPosition(const v2i& pos) const;

	// quite exposed
	std::vector<Entity*> entities;
	std::vector<AreaTrigger*> triggers;

private:
	void CreateChunk(const v2i& chunkPos);
	void CreateChunk(TileChunk* chunk);

	std::vector<TileChunk*> chunks;
	std::unordered_map<v2i, int, vecHash::KeyHash<v2i, int>, vecHash::KeyEqual<v2i>> chunkMap;
};

