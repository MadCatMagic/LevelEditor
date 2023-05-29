#include "Engine/Texture/Image.h"
#include "Engine/Texture/ImageLoader.h"
#include <iostream>

Image::Image(const std::string& filepath)
    : filepath(filepath) 
{ }

Image::~Image()
{
    if (dataLoaded)
        UnloadData();
}

unsigned char* Image::LoadData()
{
    dataPointer = ImageLoader::LoadImageData(filepath.c_str(), &width, &height, &bytesPerPixel);
    if (!dataPointer)
        std::cout << "Warning! Image data failed to load from filepath '" + filepath + "'." << std::endl;
    dataLoaded = true;
    return dataPointer;
}

void Image::UnloadData()
{
    ImageLoader::ClearImageData(dataPointer);
    dataLoaded = false;
}
