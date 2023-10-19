#include "Level/Effects.h"
#include "Level/Level.h"
#include "Engine/PixelTexture2D.h"

#include <sstream>

Effect::Effect(const std::string& displayName, const v3& tint)
	: name(displayName), editorTint(tint)
{
	effectMap.tiles = new TileMap();
}

Effect::~Effect()
{
	if (perTile)
		delete effectMap.tiles;
	else
		delete effectMap.tex;
}

EffectManager* EffectManager::instance = nullptr;

EffectManager::EffectManager()
{
	instance = this;
	ReloadEffects();
}

EffectManager::~EffectManager()
{
	for each (Effect * e in effects)
		delete e;
	effects.clear();
}

void EffectManager::ReloadEffects()
{
	for each (Effect * e in effects)
		delete e;
	effects.clear();

	effects.push_back(new Effect("Nothing Effect :)", v3(0.0f, 1.0f, 1.0f)));
	effects.push_back(new AgeEffect());
}

Effect* EffectManager::GetEffectFromName(const std::string& name)
{
	// just do linear search, few enough effects and few enough calls it is fine
	for (size_t i = 0; i < effects.size(); i++)
		if (effects[i]->name == name)
			return effects[i];

	return nullptr;
}

AgeEffect::AgeEffect()
	: Effect("Ageing", v3(0.6f, 0.8f, 0.2f))
{ 
	effectMap.tiles = new TileMap();
}

void AgeEffect::ProcessImage(const v2& bottomLeft, const v2& camSize, PixelTexture2D* normal, PixelTexture2D* colour)
{
	v2i size = v2i(normal->GetTexture()->width, normal->GetTexture()->height);
	for (int y = 0; y < size.y; y++)
		for (int x = 0; x < size.x; x++)
		{
			v4& ref = normal->At(v2i(x, y));
			int t = effectMap.tiles->GetTile((v2i)(bottomLeft + v2::Scale(camSize, v2(x / (float)size.x, y / (float)size.y))));
			ref = ref * 0.5f + v4(editorTint * (float)t / 30.0f);
		}
}

Effect::TileMap::TileMap()
{
	for each (auto & pair in Level::instance->chunkMap)
		CreateChunk(pair.first);
}

Effect::TileMap::TileMap(const v2i& chunkSizes)
{
	for (int x = 0; x < chunkSizes.x; x++)
		for (int y = 0; y < chunkSizes.y; y++)
			CreateChunk(v2i(x, y));
}

void Effect::TileMap::CreateChunk(const v2i& chunkPos)
{
	AddChunk(chunkPos, std::vector<uint64_t>(16));
}

void Effect::TileMap::AddChunk(const v2i& chunkPos, std::vector<uint64_t> chunk)
{
	if ((int)chunk.size() < 16)
		for (int i = 0; i < 16 - (int)chunk.size(); i++)
			chunk.push_back(0);
	else if ((int)chunk.size() > 16)
		for (int i = 0; i < (int)chunk.size() - 16; i++)
			chunk.pop_back();
	data.push_back(chunk);
	map[chunkPos] = data.size() - 1;
}

std::string Effect::TileMap::GetData() const
{
	std::string str;
	for each (auto& pair in map)
	{
		// seperator and chunk position
		auto& chunk = data[pair.second];
		str.append(std::to_string(pair.first.x) + "," + std::to_string(pair.first.y) + ",;");

		// actual chunk data
		for (int i = 0; i < 16; i++)
			for (int j = 0; j < 8; j++)
			{
				uint64_t num = chunk[i] >> (j * 8) & 0xffll;
				str.push_back((const char)num);
			}
	}

	return str;
}

void Effect::TileMap::SetData(const std::string& str)
{
	std::stringstream stream(str);
	char c;

	bool inPos = false;
	bool readingPosX = true;
	v2i pos;
	std::string acc;

	int counter = 0;
	size_t index = 0;

	while (stream.get(c))
	{
		if (c == ';')
			inPos = false;

		if (inPos)
		{
			if (c == ',')
			{
				if (readingPosX)
					pos.x = std::stoi(acc);
				else
					pos.y = std::stoi(acc);
				acc = "";
				readingPosX = !readingPosX;

				// create chunk
				CreateChunk(pos);
				index = map[pos];
			}
			else
				acc.push_back(c);
		}
		else
		{
			int y = counter / 8;
			int twox = counter % 8;
			data[index][y] += (uint64_t)c << (twox * 8);
			counter++;
			if (counter == 128)
			{
				inPos = true;
				counter = 0;
			}
		}
	}
}

// when you are simply getting the data, nothing matters particularly so don't bother creating any new chunks
// can return zero as a default value since any call to SetTile will create that empty value.
int Effect::TileMap::GetTile(const v2i& pos) const
{
	v2i offset;
	v2i div;
	if (ValidPosition(pos, &div, &offset))
	{
		uint64_t mask = 15ll << (offset.x * 4);
		// bitshifting has a higher precedence than binary and
		return (data[map.at(div)][offset.y] & mask) >> (offset.x * 4);
	}
	return 0;
}

// if you try and get a tile that is not a valid position, create a new chunk there in order to create that position
void Effect::TileMap::SetTile(const v2i& pos, int newValue)
{
	v2i offset;
	v2i div;

	// if not a valid position create a chunk there
	// handily the ValidPosition function fills in div and offset regardless
	if (!ValidPosition(pos, &div, &offset))
		CreateChunk(div);

	// we can always do this as there should always be a chunk there now
	uint64_t mask = 15ll << (offset.x * 4);
	data[map[div]][offset.y] = data[map[div]][offset.y] & ~mask | ((uint64_t)newValue << (offset.x * 4)) & mask;
}

void Effect::TileMap::TrimChunks()
{
	// don't want to delete chunks in the loop as it would cause iteration problems
	std::vector<v2i> chunksToDelete;

	for each (auto & pair in map)
	{
		// check if chunk is empty
		bool empty = true;
		for (int i = 0; i < TILE_CHUNK_SIZE; i++)
			if (data[pair.second][i] != 0)
				empty = false;

		// delete if empty
		if (empty)
			chunksToDelete.push_back(pair.first);
	}

	// delete all the marked chunks
	// O(n^2) but shouldnt really matter too much
	for each (const v2i& pos in chunksToDelete)
	{
		size_t index = map[pos];
		data.erase(data.begin() + index);
		map.erase(pos);
		for each (auto& pair in map)
			if (pair.second > index)
				map[pair.first]--;
	}
}

bool Effect::TileMap::ValidPosition(const v2i& pos, v2i* div, v2i* offset) const
{
	// so here, if pos.x or pos.y < 0 then the division results in div being 0,0 which is wrong, 
	// so if it is negative, subtract 16,16 to offset this
	// same reasoning for xoffset, should be 16 - offset
	v2i negative = v2i(pos.x < 0, pos.y < 0);
	*div = v2i((pos.x - 16 * negative.x) / 16, (pos.y - 16 * negative.y) / 16);
	*offset = negative * 16 + v2i(((negative.x ? -1 : 1) * pos.x) % 16, ((negative.y ? -1 : 1) * pos.y) % 16);
	return map.find(*div) != map.end();
}
