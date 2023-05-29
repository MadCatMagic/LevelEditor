#pragma once
#include "Texture2D.h"

class Cubemap : public Texture
{
public:
	Cubemap(const std::string filepaths[6]);
	Cubemap(Format format, const v2i& size, unsigned int lod);
	Cubemap(const Cubemap& obj);
	Cubemap(Cubemap&& obj) noexcept;
	~Cubemap();

	Cubemap& operator=(const Cubemap& obj);
	Cubemap& operator=(Cubemap&& obj) noexcept;

	void ApplyFiltering() const override;
	void Bind() const override;
	void Unbind() const override;

	void CreateTexture();

	inline unsigned int GetID() const { return id; }
private:
	unsigned int id;

	void GenerateID();
};

