#include "Engine/Texture/RenderTexture.h"

#include <iostream>

RenderTexture::RenderTexture(int width, int height, Format depth, Format colour)
	: Texture(Dimension::Two)
{
	this->id = -1;
	this->width = width;
	this->height = height;
	this->depthFormat = depth;
	this->colourFormat = colour;
}

RenderTexture::RenderTexture(const RenderTexture& obj)
	: Texture(Dimension::Two)
{
	this->id = obj.id;
	this->format = obj.format;
	this->formatType = obj.formatType;
	this->wrapMode = obj.wrapMode;
	this->width = obj.width;
	this->height = obj.height;
	this->lod = obj.lod;

	this->colourBuffer = obj.colourBuffer;
	this->depthBuffer = obj.depthBuffer;
	this->colourFormat = obj.colourFormat;
	this->depthFormat = obj.depthFormat;
}

RenderTexture::RenderTexture(RenderTexture&& obj) noexcept
	: Texture(Dimension::Two)
{
	this->id = obj.id;
	obj.id = -1;
	this->format = obj.format;
	obj.format = Format::RGB;
	this->formatType = obj.formatType;
	obj.formatType = Format::RGB;
	this->wrapMode = obj.wrapMode;
	obj.wrapMode = WrapMode::Repeat;
	this->width = obj.width;
	obj.width = 0;
	this->height = obj.height;
	obj.height = 0;
	this->lod = obj.lod;
	obj.lod = 0;

	this->colourBuffer = obj.colourBuffer;
	obj.colourBuffer = nullptr;
	this->depthBuffer = obj.depthBuffer;
	obj.depthBuffer = nullptr;
	this->colourFormat = obj.colourFormat;
	obj.colourFormat = Format::None;
	this->depthFormat = obj.depthFormat;
	obj.depthFormat = Format::None;
}

RenderTexture::~RenderTexture()
{
	if (generatedBuffers)
	{
		glDeleteFramebuffers(1, &id);
		if (colourFormat != Format::None)
			delete colourBuffer;
		if (depthFormat != Format::None)
			delete depthBuffer;
	}
}

// creates rendertexture and tex2d buffers
// DONT TRY TO USE RENDERTEXTURE BEFORE CALLING THIS
void RenderTexture::GenerateBuffers()
{
	// generate and bind framebuffer (render texture)
	glGenFramebuffers(1, &id);
	Bind();

	if (colourFormat != Format::None)
	{
		// create colourBuffer and assign to framebuffer
		colourBuffer = new Texture2D(colourFormat, v2i(width, height), 0);
		colourBuffer->CreateTexture();
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colourBuffer->GetID(), 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
	}
	else
		glDrawBuffer(GL_NONE);

	if (depthFormat != Format::None)
	{
		// create depthBuffer amd assign to framebuffer
		depthBuffer = new Texture2D(depthFormat, v2i(width, height), 0);
		depthBuffer->CreateTexture();
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer->GetID(), 0);
	}

	generatedBuffers = true;
	if (!TextureOK())
		std::cout << "Rendertexture error!" << std::endl;
	Unbind();
}

// makes sure nothing is broken
bool RenderTexture::TextureOK() const
{
	Bind();
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;
	return true;
}

RenderTexture& RenderTexture::operator=(const RenderTexture& obj)
{
	this->id = obj.id;
	this->format = obj.format;
	this->formatType = obj.formatType;
	this->wrapMode = obj.wrapMode;
	this->width = obj.width;
	this->height = obj.height;
	this->lod = obj.lod;

	this->colourBuffer = obj.colourBuffer;
	this->depthBuffer = obj.depthBuffer;
	this->colourFormat = obj.colourFormat;
	this->depthFormat = obj.depthFormat;
	return *this;
}

RenderTexture& RenderTexture::operator=(RenderTexture&& obj) noexcept
{
	this->id = obj.id;
	obj.id = -1;
	this->format = obj.format;
	obj.format = Format::RGB;
	this->formatType = obj.formatType;
	obj.formatType = Format::RGB;
	this->wrapMode = obj.wrapMode;
	obj.wrapMode = WrapMode::Repeat;
	this->width = obj.width;
	obj.width = 0;
	this->height = obj.height;
	obj.height = 0;
	this->lod = obj.lod;
	obj.lod = 0;

	this->colourBuffer = obj.colourBuffer;
	obj.colourBuffer = nullptr;
	this->depthBuffer = obj.depthBuffer;
	obj.depthBuffer = nullptr;
	this->colourFormat = obj.colourFormat;
	obj.colourFormat = Format::None;
	this->depthFormat = obj.depthFormat;
	obj.depthFormat = Format::None;
	return *this;
}

void RenderTexture::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void RenderTexture::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}