#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Vector.h"

class Texture2D;
class RenderTexture;
class Light;
class Material;

namespace Renderer
{
	extern void DebugEnable();
	extern void Init(bool debugEnable, GLFWwindow* window);
	extern v2i GetWinSize();

	extern void ClearScreen(GLbitfield mask);
	extern void Viewport(const v2i& size);
	extern void Viewport(const v2i& size, const v2i& offset);
	extern void BindScreenBuffer();

	extern void Render();

	/* Blit functions */
	// leave rendertexture as nullptr to blit to screen
	// using Texture2Ds
	extern void Blit(const Texture2D* src, RenderTexture* dest);
	extern void Blit(const Texture2D* src, RenderTexture* dest, Material* mat);
	extern void Blit(const Texture2D* src, const Texture2D* depth, RenderTexture* dest);				 // includes depth texture
	extern void Blit(const Texture2D* src, const Texture2D* depth, RenderTexture* dest, Material* mat); // includes depth texture
	// using RenderTextures
	extern void Blit(const RenderTexture* src, RenderTexture* dest, bool drawDepth);
	extern void Blit(const RenderTexture* src, RenderTexture* dest, Material* mat, bool drawDepth);

	


	// releases any heap variables
	extern void Release();
}