#pragma once
#include <GL/glew.h>
#include "Vector.h"

class Texture
{
public:
	enum class Format {
		None = -1,
		RGB = GL_RGB, RGB8 = GL_RGB8, RGBFloat = GL_RGB32F,
		RGBA = GL_RGBA, RGBA8 = GL_RGBA8, RGBAFloat = GL_RGBA32F,
		Red = GL_RED,
		Depth = GL_DEPTH_COMPONENT, Depth16 = GL_DEPTH_COMPONENT16, Depth24 = GL_DEPTH_COMPONENT24
	};

	enum class WrapMode {
		Repeat = GL_REPEAT, Clamp = GL_CLAMP_TO_EDGE, Mirror = GL_MIRRORED_REPEAT, Border = GL_CLAMP_TO_BORDER
	};

	enum class Sampling {
		Nearest = GL_NEAREST, Linear = GL_LINEAR
	};

	enum class Dimension {
		Two = 2, Three = 3, Cubemap = 6
	};

	Texture(Dimension dim) : dimension(dim) {}

	Format format{ Format::RGBA };
	Format formatType{ Format::RGBA };
	WrapMode wrapMode{ WrapMode::Repeat };
	Sampling sampling{ Sampling::Linear };

	Dimension dimension;
	int width{ 0 };
	int height{ 0 };
	int depth{ 0 };

	unsigned int lod{ 0 };

	inline virtual void ApplyFiltering() const {};
	inline virtual void Bind() const {};
	inline virtual void Unbind() const {};
	inline virtual void SetBorderColour(const v4& colour) const {};

	inline static void BindTextureUnit(int unit) { glActiveTexture(GL_TEXTURE0 + unit); }
	inline static void UnbindTextureUnit() { glActiveTexture(GL_TEXTURE0); }

protected:
	static Format FormatType(Format format);
};