#pragma once
#include "Texture2D.h"

class RenderTexture : public Texture
{
public:

	// both depth and colour
	RenderTexture(int width, int height, Format depth, Format colour);
	RenderTexture(const RenderTexture& obj);
	RenderTexture(RenderTexture&& obj) noexcept;
	~RenderTexture();

	void GenerateBuffers();

	bool TextureOK() const;

	Texture2D* colourBuffer{ nullptr };
	Texture2D* depthBuffer{ nullptr };

	RenderTexture& operator=(const RenderTexture& obj);
	RenderTexture& operator=(RenderTexture&& obj) noexcept;

	void Bind() const override;
	void Unbind() const override;

	inline unsigned int GetID() { return id; };
private:
	Format depthFormat;
	Format colourFormat;

	unsigned int id;
	bool generatedBuffers{ false };
}; 