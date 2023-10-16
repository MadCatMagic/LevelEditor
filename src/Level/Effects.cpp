#include "Level/Effects.h"

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
	effects.push_back(new Effect("Nothing Effect :)", v3(0.0f, 1.0f, 1.0f)));
	effects.push_back(new AgeEffect());
	instance = this;
}

EffectManager::~EffectManager()
{
	for each (Effect * e in effects)
		delete e;
	effects.clear();
}

AgeEffect::AgeEffect()
	: Effect("Ageing", v3(0.6f, 0.8f, 0.2f))
{ 
	effectMap.tiles = new TileMap();
}

#include "Engine/PixelTexture2D.h"
void AgeEffect::ProcessImage(const v2& bottomLeft, const v2& camSize, PixelTexture2D* normal, PixelTexture2D* colour)
{
	v2i size = v2i(normal->GetTexture()->width, normal->GetTexture()->height);
	for (int y = 0; y < size.y; y++)
		for (int x = 0; x < size.x; x++)
		{
			v4& ref = normal->At(v2i(x, y));
			int t = effectMap.tiles->GetTile((v2i)(bottomLeft + v2::Scale(camSize, v2(x / (float)size.x, y / (float)size.y)) * 0.5f));
			ref = ref * 0.5f + v4(editorTint * t / 30.0f);
		}
}

#include "Level/Level.h"
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
	if (chunk.size() < 16)
		for (int i = 0; i < 16 - chunk.size(); i++)
			chunk.push_back(0);
	else if (chunk.size() > 16)
		for (int i = 0; i < chunk.size() - 16; i++)
			chunk.pop_back();
	data.push_back(chunk);
	map[chunkPos] = data.size() - 1;
}

std::string Effect::TileMap::GetData() const
{
	return "";
}

void Effect::TileMap::SetData(const std::string& str)
{
}

int Effect::TileMap::GetTile(const v2i& pos)
{
	v2i offset;
	v2i div;
	if (ValidPosition(pos, &div, &offset))
	{
		uint64_t mask = 15ll << (offset.x * 4);
		return data[map[div]][offset.y] & mask >> (offset.x * 4);
	}
	return -1;
}

void Effect::TileMap::SetTile(const v2i& pos, int newValue)
{
	v2i offset;
	v2i div;
	if (ValidPosition(pos, &div, &offset))
	{
		uint64_t mask = 15ll << (offset.x * 4);
		data[map[div]][offset.y] = data[map[div]][offset.y] ^ mask | ((uint64_t)newValue << (offset.x * 4)) & mask;
	}
}

bool Effect::TileMap::ValidPosition(const v2i& pos, v2i* div, v2i* xoffset)
{
	*div = v2i(pos.x / 16, pos.y / 16);
	*xoffset = v2i(pos.x % 16, pos.y % 16);
	return map.find(*div) != map.end();
}
