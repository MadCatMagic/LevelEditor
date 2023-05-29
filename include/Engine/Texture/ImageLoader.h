#pragma once

namespace ImageLoader
{
	extern unsigned char* LoadImageData(const char* filepath, int* width, int* height, int* bytesPerPixel);
	extern void ClearImageData(unsigned char* data);
};

