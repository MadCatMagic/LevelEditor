#pragma once
#include "Vector.h"
#include <vector>
#include <unordered_map>

struct Effect
{
	Effect(const std::string& displayName, const v3& editorTint);
	~Effect();
	std::string name;
	v3 editorTint;
	
	bool perTile = true;

	virtual void ProcessImage(class PixelTexture2D* normal, PixelTexture2D* colour) { };

	class GranularMap
	{
		
	};

	class TileMap
	{
	public:

		TileMap();
		TileMap(const v2i& chunkSizes);

		void CreateChunk(const v2i& chunkPos);
		void AddChunk(const v2i& chunkPos, std::vector<uint64_t> chunk);

		std::string GetData() const;
		void SetData(const std::string& str);

		// returns 0-15
		int GetTile(const v2i& pos);
		// newValue must be 0-15
		void SetTile(const v2i& pos, int newValue);

	private:
		bool ValidPosition(const v2i& pos, v2i* div, v2i* xoffset);

		std::vector<std::vector<uint64_t>> data;
		std::unordered_map<v2i, size_t, vecHash::KeyHash<v2i, int>, vecHash::KeyEqual<v2i>> map;
	};

	union effectMapping
	{
		GranularMap* tex;
		TileMap* tiles;
	} effectMap;
};

class EffectManager
{
public:
	static EffectManager* instance;

	EffectManager();
	~EffectManager();

	inline size_t GetNumEffects() { return effects.size(); }
	inline Effect* GetEffect(int index) { return effects[index]; }
	
private:
	std::vector<Effect*> effects;
};

struct AgeEffect : public Effect
{
	AgeEffect();
	
	void ProcessImage(const v2& bottomLeft, const v2& camSize, class PixelTexture2D* normal, PixelTexture2D* colour) override;
};