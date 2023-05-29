#include "Engine/Texture/Texture2D.h"
#include "Engine/Texture/Image.h"

// quite basic texture importing
// might improve later on
Texture2D::Texture2D(const std::string& filepath)
	: Texture(Dimension::Two)
{
	id = -1;
	Image image = Image(filepath);
	unsigned char* imageData = image.LoadData();
	this->width = image.GetWidth();
	this->height = image.GetHeight();
	this->format = image.GetBytesPerPixel() == 3 ? Format::RGB : Format::RGBA;
	this->formatType = FormatType(format);
	this->lod = 0;
	CreateTexture(imageData);
	image.~Image();
}

Texture2D::Texture2D(Format format, const v2i& size, unsigned int lod)
	: Texture(Dimension::Two)
{
	id = -1;
	this->format = format;
	this->formatType = FormatType(format);
	this->width = size.x;
	this->height = size.y;
	this->lod = lod;
}

Texture2D::Texture2D(Format format, const v2i& size, WrapMode wrapMode, unsigned int lod)
	: Texture(Dimension::Two)
{
	id = -1;
	this->format = format;
	this->formatType = FormatType(format);
	this->width = size.x;
	this->height = size.y;
	this->lod = lod;
	this->wrapMode = wrapMode;
}

Texture2D::Texture2D(Format format, const v2i& size, unsigned int lod, const void* data)
	: Texture(Dimension::Two)
{
	id = -1;
	this->format = format;
	this->formatType = FormatType(format);
	this->width = size.x;
	this->height = size.y;
	this->lod = lod;
	CreateTexture(data);
}

Texture2D::Texture2D(Format format, const v2i& size, WrapMode wrapMode, unsigned int lod, const void* data)
	: Texture(Dimension::Two)
{
	id = -1;
	this->format = format;
	this->formatType = FormatType(format);
	this->width = size.x;
	this->height = size.y;
	this->lod = lod;
	this->wrapMode = wrapMode;
	CreateTexture(data);
}

Texture2D::Texture2D(const Texture2D& obj)
	: Texture(Dimension::Two)
{
	this->id = obj.id;
	this->format = obj.format;
	this->formatType = obj.formatType;
	this->wrapMode = obj.wrapMode;
	this->width = obj.width;
	this->height = obj.height;
	this->lod = obj.lod;
}

Texture2D::Texture2D(Texture2D&& obj) noexcept
	: Texture(Dimension::Two)
{
	this->id = obj.id;
	obj.id = -1;
	this->format = obj.format;
	obj.format = Format::RGBA;
	this->formatType = obj.formatType;
	obj.formatType = Format::RGBA;
	this->wrapMode = obj.wrapMode;
	obj.wrapMode = WrapMode::Repeat;
	this->width = obj.width;
	obj.width = 0;
	this->height = obj.height;
	obj.height = 0;
	this->lod = obj.lod;
	obj.lod = 0;
}

Texture2D::~Texture2D()
{
	if (id != -1)
		glDeleteTextures(1, &id);
}

Texture2D& Texture2D::operator=(const Texture2D& other)
{
	this->id = other.id;
	this->format = other.format;
	this->formatType = other.formatType;
	this->wrapMode = other.wrapMode;
	this->width = other.width;
	this->height = other.height;
	this->lod = other.lod;
	return *this;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept
{
	this->id = other.id;
	other.id = -1;
	this->format = other.format;
	other.format = Format::RGBA;
	this->formatType = other.formatType;
	other.formatType = Format::RGBA;
	this->wrapMode = other.wrapMode;
	other.wrapMode = WrapMode::Repeat;
	this->width = other.width;
	other.width = 0;
	this->height = other.height;
	other.height = 0;
	this->lod = other.lod;
	other.lod = 0;
	return *this;
}

// make sure texture is bound AND created
void Texture2D::ApplyFiltering() const
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int)wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int)wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)sampling);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)sampling);
}

void Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2D::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

// make sure texture is bound AND created
void Texture2D::SetBorderColour(const v4& colour) const
{
	float borderColor[] = { colour.x, colour.y, colour.z, colour.w };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void Texture2D::CreateTexture()
{
	glGenTextures(1, &id);
	Bind();
	glTexImage2D(GL_TEXTURE_2D, lod, (int)format, width, height, 0, (int)formatType, GL_UNSIGNED_BYTE, 0);
	ApplyFiltering();
}

void Texture2D::CreateTexture(const void* data)
{
	glGenTextures(1, &id);
	Bind();
	glTexImage2D(GL_TEXTURE_2D, lod, (int)format, width, height, 0, (int)formatType, GL_UNSIGNED_BYTE, data);
	ApplyFiltering();
}

void Texture2D::SubTexture(const void* data)
{
	SubTexture(data, v2i(width, height), v2i::zero, 0);
}

void Texture2D::SubTexture(const void* data, const v2i& size, const v2i& offset)
{
	SubTexture(data, size, offset, 0);
}

void Texture2D::SubTexture(const void* data, const v2i& size, const v2i& offset, int lod)
{
	glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x, offset.y, size.x, size.y, (GLenum)format, GL_FLOAT, data);
}

float* Texture2D::ReadTexture()
{
	return ReadTexture(0);
}

float* Texture2D::ReadTexture(int lod)
{
	void* ptr = nullptr;
	glGetTexImage(GL_TEXTURE_2D, lod, (GLenum)format, GL_UNSIGNED_BYTE, ptr);
	return (float*)ptr;
}

