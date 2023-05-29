#include "Engine/Texture/Cubemap.h"
#include "Engine/Texture/Image.h"

Cubemap::Cubemap(const std::string filepaths[6])
    : Texture(Dimension::Cubemap)
{
    this->id = -1;
    GenerateID();
    for (unsigned int i = 0; i < 6; i++)
    {
        Image data = Image(filepaths[i]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.LoadData()
        );
        this->width = data.GetWidth();
        this->height = data.GetHeight();
        this->format = data.GetBytesPerPixel() == 3 ? Format::RGB : Format::RGBA;
        data.~Image();
    }
    this->formatType = FormatType(format);
    this->lod = 0;
    ApplyFiltering();
}

Cubemap::Cubemap(Format format, const v2i& size, unsigned int lod)
    : Texture(Dimension::Cubemap)
{
    this->id = -1;
    this->format = format;
    this->formatType = FormatType(format);
    this->wrapMode = WrapMode::Clamp;
    this->width = size.x;
    this->height = size.y;
    this->lod = lod;
}

Cubemap::Cubemap(const Cubemap& obj)
    : Texture(Dimension::Cubemap)
{
    this->id = obj.id;
    this->format = obj.format;
    this->formatType = obj.formatType;
    this->wrapMode = obj.wrapMode;
    this->width = obj.width;
    this->height = obj.height;
    this->lod = obj.lod;
}

Cubemap::Cubemap(Cubemap&& obj) noexcept
    : Texture(Dimension::Cubemap)
{
    this->id = obj.id;
    obj.id = -1;
    this->format = obj.format;
    obj.format = Format::RGB;
    this->formatType = obj.formatType;
    obj.formatType = Format::RGB;
    this->wrapMode = obj.wrapMode;
    obj.wrapMode = WrapMode::Clamp;
    this->width = obj.width;
    obj.width = 0;
    this->height = obj.height;
    obj.height = 0;
    this->lod = obj.lod;
    obj.lod = 0;
}

Cubemap::~Cubemap()
{
    if (id != -1)
        glDeleteTextures(1, &id);
}

Cubemap& Cubemap::operator=(const Cubemap& obj)
{
    this->id = obj.id;
    this->format = obj.format;
    this->formatType = obj.formatType;
    this->wrapMode = obj.wrapMode;
    this->width = obj.width;
    this->height = obj.height;
    this->lod = obj.lod;
    return *this;
}

Cubemap& Cubemap::operator=(Cubemap&& obj) noexcept
{
    this->id = obj.id;
    obj.id = -1;
    this->format = obj.format;
    obj.format = Format::RGB;
    this->formatType = obj.formatType;
    obj.formatType = Format::RGB;
    this->wrapMode = obj.wrapMode;
    obj.wrapMode = WrapMode::Clamp;
    this->width = obj.width;
    obj.width = 0;
    this->height = obj.height;
    obj.height = 0;
    this->lod = obj.lod;
    obj.lod = 0;
    return *this;
}

void Cubemap::ApplyFiltering() const
{
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, (int)sampling);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (int)sampling);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, (int)wrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, (int)wrapMode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, (int)wrapMode);
}

void Cubemap::Bind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void Cubemap::Unbind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Cubemap::CreateTexture()
{
    GenerateID();
    for (unsigned int i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, (int)format, width, height, 0, (int)formatType, GL_UNSIGNED_BYTE, 0);
    ApplyFiltering();
}

void Cubemap::GenerateID()
{
    glGenTextures(1, &id);
    Bind();
}
