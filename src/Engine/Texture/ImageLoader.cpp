#include "Engine/Texture/ImageLoader.h"

// open source image importing library
#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push, 0)
#include "stb_image.h"
#pragma warning(pop)

unsigned char* ImageLoader::LoadImageData(const char* filepath, int* width, int* height, int* bytesPerPixel)
{
    return stbi_load(filepath, width, height, bytesPerPixel, 0);
}

void ImageLoader::ClearImageData(unsigned char* data)
{
    stbi_image_free(data);
}
