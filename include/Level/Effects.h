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

	virtual void ProcessImage(const v2& bottomLeft, const v2& camSize, class PixelTexture2D* normal, PixelTexture2D* colour) { };

	class GranularMap
	{
		// todo
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
		int GetTile(const v2i& pos) const;
		// newValue must be 0-15
		void SetTile(const v2i& pos, int newValue);

		std::unordered_map<v2i, size_t, vecHash::KeyHash<v2i, int>, vecHash::KeyEqual<v2i>> map;

		// call at the end of every frame to make sure that no chunks are empty/delete them if they are
		// maybe in future if there are too many chunks could optimise by adding an extra flag for when a value is set in a chunk
		// and only check the chunks with that flag set
		void TrimChunks();

	private:
		bool ValidPosition(const v2i& pos, v2i* div, v2i* offset) const;

		std::vector<std::vector<uint64_t>> data;
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
	
	void ProcessImage(const v2& bottomLeft, const v2& camSize, PixelTexture2D* normal, PixelTexture2D* colour) override;
};