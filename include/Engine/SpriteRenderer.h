#pragma once
#include "Engine/Texture.h"
#include <vector>
#include <unordered_map>

class SpriteRenderer
{
public:
	SpriteRenderer(int drawlevel = 0);
	~SpriteRenderer();

	void SetTexture(Texture2D* tex);
	void SetTexture(RenderTexture* tex);
	void Clear();

	void SetPixelPos(v2 pixelPos);

	void SetScale(v2 scale);

	void SetRotation(float degrees);
	void SetRotation(int tileRotation); // 0 to 3 (mod 4) for multiples of 90

	void SetLayer(int l);

	unsigned int GetTextureID() const;
	v2i GetTextureSize() const;

	static void RenderAll(RenderTexture* dest);
	static void Initialise();
	static void Release();

	static v2i pixelScreenSize;

	v4 tint;
	bool render;

private:
	void Render(); // returns textureid
	
	static int RenderLayer(int startingIndex);

	// cannot be both at once
	Texture2D* tex2D = nullptr;
	RenderTexture* rt = nullptr;

	int layer;
	v2 pos;
	v2 scale = v2::one;

	v2i pixelSize;
	
	// radians
	float rotation = 0.0f;
	int tileRot = -1;

	// instancing
	static std::unordered_map<int, std::vector<float>> instancedData;

	static std::vector<SpriteRenderer*> renderers;
};

