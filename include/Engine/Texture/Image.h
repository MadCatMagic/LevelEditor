#pragma once
#include "Vector.h"

class Image
{
public:
	Image(const std::string& filepath);
	~Image();

	// use to load data from the filepath
	// expensive operation
	// returns a pointer to the data
	unsigned char* LoadData();
	// clears data from the pointer
	void UnloadData();

	inline unsigned char* GetData() const { if (dataLoaded) return dataPointer; else return nullptr; }
	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }
	inline int GetBytesPerPixel() const { return bytesPerPixel; }
	
	v4 At(const v2i& pos) const;
	v4 At(int index) const;

private:
	std::string filepath;
	unsigned char* dataPointer{ nullptr };
	bool dataLoaded{ false };

	int width{ 0 };
	int height{ 0 };
	int bytesPerPixel{ 0 };
};

