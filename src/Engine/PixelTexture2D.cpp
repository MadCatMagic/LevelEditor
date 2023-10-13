#include "Engine/PixelTexture2D.h"

PixelTexture2D::PixelTexture2D()
{ }

PixelTexture2D::PixelTexture2D(Texture2D* target)
	: target(target)
{ 
	ReadDataFromTexture();
}

PixelTexture2D::~PixelTexture2D()
{
	if (createdOwnTexture)
		delete target;
	if (textureData != nullptr)
		delete[] textureData;
}

void PixelTexture2D::CreateTexture(const v2i& size)
{
	target = new Texture2D(Texture::Format::RGBA, size, 0);
	target->CreateTexture();
	createdOwnTexture = true;
	CreateArray();
}

void PixelTexture2D::SetTarget(Texture2D* target)
{
	if (target != nullptr && createdOwnTexture)
		delete this->target;
	this->target = target;
	createdOwnTexture = false;
}

void PixelTexture2D::UpdateTexture()
{
	target->Bind();
	target->SubTexture((const void*)textureData);
}

void PixelTexture2D::UpdateTexture(v4* textureData)
{
	target->Bind();
	target->SubTexture((const void*)textureData);
}

void PixelTexture2D::SetSubTexture(v4* textureData, const v2i& offset, const v2i& size)
{
	target->Bind();
	target->SubTexture((const void*)textureData, size, offset);
}

void PixelTexture2D::ReadDataFromTexture()
{
	target->Bind();
	float* data = target->ReadTexture();
	CreateArray((void*)data);
}

void PixelTexture2D::CreateArray()
{
	CreateArray(nullptr);
}

void PixelTexture2D::CreateArray(void* data)
{
	if (textureData != nullptr)
		delete[] textureData;
	textureData = new v4[target->width * target->height];
}
