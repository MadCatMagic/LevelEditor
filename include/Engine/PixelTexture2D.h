#pragma once
#include "Texture/Texture2D.h"

class PixelTexture2D
{
public:
	PixelTexture2D();
	PixelTexture2D(Texture2D* target);
	~PixelTexture2D();
	
	void CreateTexture(const v2i& size);
	void SetTarget(Texture2D* target);

	void UpdateTexture();
	void UpdateTexture(v3* textureData);
	void SetSubTexture(v3* textureData, const v2i& offset, const v2i& size);

	inline int CoordToIndex(const v2i& coord) const { return coord.x + coord.y * target->width; }
	inline v2i IndexToCoord(int index) const { return v2i(index % target->width, index / target->width); }
	inline bool InBounds(const v2i& coord) const { return coord.x >= 0 && coord.x < target->width && coord.y >= 0 && coord.y < target->height; }

	inline v3* GetRawData() { return textureData; }
	inline Texture2D* GetTexture() { return target; }

private:
	void ReadDataFromTexture();
	void CreateArray();
	void CreateArray(void* data);
	Texture2D* target = nullptr;
	
	v3* textureData = nullptr;

	bool createdOwnTexture = false;
};

