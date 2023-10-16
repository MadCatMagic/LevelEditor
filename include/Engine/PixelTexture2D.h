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
	void UpdateTexture(v4* textureData);
	void SetSubTexture(v4* textureData, const v2i& offset, const v2i& size);

	inline int CoordToIndex(const v2i& coord) const { return coord.x + coord.y * target->width; }
	inline v2i IndexToCoord(int index) const { return v2i(index % target->width, index / target->width); }
	inline bool InBounds(const v2i& coord) const { return coord.x >= 0 && coord.x < target->width && coord.y >= 0 && coord.y < target->height; }

	inline v4* GetRawData() { return textureData; }
	inline Texture2D* GetTexture() { return target; }

	inline v4& At(int index) const { return textureData[index]; }
	inline v4& At(const v2i& coord) const { return textureData[CoordToIndex(coord)]; }

private:
	void ReadDataFromTexture();
	void CreateArray();
	void CreateArray(void* data);
	Texture2D* target = nullptr;
	
	v4* textureData = nullptr;

	bool createdOwnTexture = false;
};

