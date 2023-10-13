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

v4 Image::At(const v2i& pos) const
{
    return At(pos.x + pos.y * width);
}

v4 Image::At(int index) const
{
    int k = index * bytesPerPixel;
    if (bytesPerPixel == 3)
        return v3((float)dataPointer[k] / 255.0f, (float)dataPointer[k + 1] / 255.0f, (float)dataPointer[k + 2] / 255.0f);
    else if (bytesPerPixel == 4)
        return v4((float)dataPointer[k] / 255.0f, (float)dataPointer[k + 1] / 255.0f, (float)dataPointer[k + 2] / 255.0f, (float)dataPointer[k + 3] / 255.0f);
    return 0;
}