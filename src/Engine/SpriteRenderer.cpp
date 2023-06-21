#include "Engine/SpriteRenderer.h"
#include "Engine/Renderer.h"

#include "Engine/Mesh/Material.h"
#include "Engine/Mesh/Shader.h"
#include "Engine/Mesh/VertexArray.h"
#include "Engine/Mesh/VertexBuffer.h"

#include <iostream>

// render stuff
static const v2 blitQuadData[6] = {
	v2(-1.0f, -1.0f),
	v2( 1.0f, -1.0f),
	v2(-1.0f,  1.0f),
	v2(-1.0f,  1.0f),
	v2( 1.0f, -1.0f),
	v2( 1.0f,  1.0f),
};

static VertexBuffer* blitVB;
static VertexArray* blitVA;
static Shader* blitShader;
static Material* blitMat;

// things to be drawn first are at the front with the highest layer value
std::vector<SpriteRenderer*> SpriteRenderer::renderers = std::vector<SpriteRenderer*>();

v2i SpriteRenderer::pixelScreenSize = v2i(1, 1);

SpriteRenderer::SpriteRenderer(int drawlevel)
	: tint(v4::one), render(true)
{
	layer = drawlevel;
	renderers.push_back(this);

	// need to ensure it is inserted at the correct level
	int i = renderers.size() - 1;
	while (i > 0)
	{
		// if layer is the same or greater than us then break
		if (renderers[i]->layer <= renderers[i - 1]->layer)
			break;
		// if their layer is smaller than ours then swap
		else
		{
			SpriteRenderer* temp = renderers[i - 1];
			renderers[i - 1] = renderers[i];
			renderers[i] = temp;
		}
		i--;
	}
}

// linear search
// can do binary search but isnt as efficient for small lengths of vector especially for a one time operation
SpriteRenderer::~SpriteRenderer()
{
	for (unsigned int i = 0; i < renderers.size(); i++)
	{
		if (renderers[i] == this)
			renderers.erase(renderers.begin() + i);
	}
}

void SpriteRenderer::SetTexture(Texture2D* tex)
{
	tex2D = tex;
	rt = nullptr;
	pixelSize = v2i(tex->width, tex->height);
}

void SpriteRenderer::SetTexture(RenderTexture* tex)
{
	tex2D = nullptr;
	rt = tex;
	pixelSize = v2i(tex->width, tex->height);
}

void SpriteRenderer::Clear() 
{
	tex2D = nullptr;
	rt = nullptr;
	pixelSize = v2i();
}

void SpriteRenderer::Render(unsigned int target)
{
	if (!render)
		return;

	unsigned int src;
	v2i dimensions;
	if (rt != nullptr)
	{
		src = rt->colourBuffer->GetID();
		dimensions = v2i(rt->width, rt->height);
	}
	else if (tex2D != nullptr)
	{
		src = tex2D->GetID();
		dimensions = v2i(tex2D->width, tex2D->height);
	}
	else
		return;
	
	v2 realscale = v2::Scale(v2((float)dimensions.x / (float)pixelScreenSize.x, (float)dimensions.y / (float)pixelScreenSize.y), scale);

	// target position
	v2 realpos = (v2)pos * 2.0f - (v2)pixelScreenSize + (v2)pixelSize;
	// scale to 0-1
	realpos = v2(realpos.x / pixelScreenSize.x, realpos.y / pixelScreenSize.y);

	float quad[18]{};
	for (int i = 0; i < 6; i++)
	{
		quad[i * 3] = blitQuadData[i].x * realscale.x + realpos.x;
		quad[i * 3 + 1] = blitQuadData[i].y * realscale.y + realpos.y;
		quad[i * 3 + 2] = 0.0f;
	}

	// Use the shader
	blitMat->Bind();
	blitMat->SetVector4("tint", tint);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, src);
	// Set our "renderedTexture" sampler to use Texture Unit 0
	blitMat->SetTexture("blitTexture", 0);

	// 1rst attribute buffer : vertices
	blitVB->Bind();
	blitVB->SetData(quad, sizeof(float) * 18, VertexBuffer::UsageHint::StreamDraw);
	blitVA->Bind();
	blitVA->EnableAttribute(0);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6);
	blitVA->DisableAttribute(0);
}

void SpriteRenderer::SetPos(v2 position)
{
	//pos = position;
	std::cerr << "unimplemented! dont use SetPos on SpriteRenderer!" << std::endl;
}

// top left
void SpriteRenderer::SetPixelPos(v2 pixelPos)
{
	pos = pixelPos;
}

void SpriteRenderer::SetScale(v2 scale)
{
	this->scale = scale;
}

void SpriteRenderer::SetPixelSizing(v2i size)
{
}

// weird one-unit bubble sort kinda
void SpriteRenderer::SetLayer(int l)
{
	layer = l;
	
	// find this renderer
	unsigned int i = 0;
	for (; i < renderers.size(); i++)
	{
		if (renderers[i] == this)
			break;
	}

	// move to correct location
	while (i > 0 && i < renderers.size() - 1)
	{
		// if needs to move down then swap
		if (renderers[i]->layer > renderers[i - 1]->layer)
		{
			SpriteRenderer* temp = renderers[i - 1];
			renderers[i - 1] = renderers[i];
			renderers[i] = temp;
		}
		// if needs to move up then swap
		else if (renderers[i]->layer < renderers[i + 1]->layer)
		{
			SpriteRenderer* temp = renderers[i + 1];
			renderers[i + 1] = renderers[i];
			renderers[i] = temp;
		}
		// else fine
		else
			break;
	}
}

void SpriteRenderer::RenderAll(RenderTexture* dest)
{
	unsigned int target;
	if (dest == nullptr)
		target = 0;
	else
		target = dest->GetID();

	// Render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, target);
	Renderer::Viewport(Renderer::GetWinSize());

	for (SpriteRenderer* r : renderers)
		r->Render(target);
}

void SpriteRenderer::Initialise()
{
	// setup blit stuff
	blitVB = new VertexBuffer(nullptr, 0);
	blitVA = new VertexArray();
	blitVA->Construct();
	blitVA->EnableAttribute(0);
	blitVA->FormatAttribute(0, 3, GL_FLOAT, false, 0, 0);
	blitVA->DisableAttribute(0);
	// Create and compile our GLSL program from the shaders
	blitShader = new Shader("res/shaders/Blit.shader");
	blitMat = new Material(*blitShader);
}

void SpriteRenderer::Release()
{
	delete blitVB;
	delete blitVA;
	delete blitShader;
	delete blitMat;
}
