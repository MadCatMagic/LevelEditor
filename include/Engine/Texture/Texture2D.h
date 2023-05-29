#pragma once
#include "Texture.h"

class Texture2D : public Texture
{
public:
	Texture2D(const std::string& filepath);
	Texture2D(Format format, const v2i& size, unsigned int lod);
	Texture2D(Format format, const v2i& size, WrapMode wrapMode, unsigned int lod);
	Texture2D(Format format, const v2i& size, unsigned int lod, const void* data);
	Texture2D(Format format, const v2i& size, WrapMode wrapMode, unsigned int lod, const void* data);
	Texture2D(const Texture2D& obj);
	Texture2D(Texture2D&& obj) noexcept;
	~Texture2D();

	Texture2D& operator=(const Texture2D& other);
	Texture2D& operator=(Texture2D&& other) noexcept;

	void ApplyFiltering() const override;
	void Bind() const override;
	void Unbind() const override;
	void SetBorderColour(const v4& colour) const override;

	void CreateTexture();
	void CreateTexture(const void* data);

	// assumes data is the size of the image
	void SubTexture(const void* data);
	void SubTexture(const void* data, const v2i& size, const v2i& offset);
	void SubTexture(const void* data, const v2i& size, const v2i& offset, int lod);

	// returns whole image 
	float* ReadTexture();
	float* ReadTexture(int lod);

	inline unsigned int GetID() const { return id; }
private:
	unsigned int id;
};