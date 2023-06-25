#include "Engine/SpriteRenderer.h"
#include "Engine/Renderer.h"

#include "Engine/Mesh/Material.h"
#include "Engine/Mesh/Shader.h"
#include "Engine/Mesh/VertexArray.h"
#include "Engine/Mesh/VertexBuffer.h"

#include <iostream>

// render stuff, xy are positions, zw are tex coords
static const v4 blitQuadData[6] = {
	v4(-1.0f, -1.0f, 0.0f, 1.0f),
	v4( 1.0f, -1.0f, 1.0f, 1.0f),
	v4(-1.0f,  1.0f, 0.0f, 0.0f),
	v4(-1.0f,  1.0f, 0.0f, 0.0f),
	v4( 1.0f, -1.0f, 1.0f, 1.0f),
	v4( 1.0f,  1.0f, 1.0f, 0.0f),
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
	
	float quad[30]{};
	for (int i = 0; i < 6; i++)
	{
		// simple rotation
		float sangle = sinf(rotation);
		float cangle = cosf(rotation);

		v2 quadData = v2(
			cangle * blitQuadData[i].x - sangle * blitQuadData[i].y,
			sangle * blitQuadData[i].x + cangle * blitQuadData[i].y
		);
		
		quad[i * 5] = quadData.x * realscale.x + realpos.x;
		quad[i * 5 + 1] = quadData.y * realscale.y + realpos.y;
		quad[i * 5 + 2] = 0.0f;
		quad[i * 5 + 3] = blitQuadData[i].z;
		quad[i * 5 + 4] = blitQuadData[i].w;
	}

	// Use the shader
	blitMat->SetVector4("tint", tint);
	
	glBindTexture(GL_TEXTURE_2D, src);

	// 1rst attribute buffer : vertices
	blitVB->SetData(quad, sizeof(float) * 30, VertexBuffer::UsageHint::StreamDraw);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, 6);
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

void SpriteRenderer::SetRotation(float degrees)
{
	rotation = degrees * 0.01745329251f; // / 360 * 2pi
	tileRot = (int)degrees / 90;
	if (degrees - (float)tileRot * 90.0f != 0.0f)
		tileRot = -1;
}

void SpriteRenderer::SetRotation(int tileRotation)
{
	tileRot = tileRotation;
	rotation = tileRotation * 90.0f * 0.01745329251f;
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

	// moved lots of blit calls outside of the render function
	blitMat->Bind();
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	// Set our "renderedTexture" sampler to use Texture Unit 0
	blitMat->SetTexture("blitTexture", 0);

	blitVB->Bind();
	blitVA->Bind();
	blitVA->EnableAttribute(0);
	blitVA->EnableAttribute(1);

	for (SpriteRenderer* r : renderers)
		r->Render(target);

	blitVA->DisableAttribute(0);
	blitVA->DisableAttribute(1);
}

void SpriteRenderer::Initialise()
{
	// setup blit stuff
	blitVB = new VertexBuffer(nullptr, 0);
	blitVA = new VertexArray();
	blitVA->Construct();
	blitVA->EnableAttribute(0);
	blitVA->EnableAttribute(1);
	blitVA->FormatAttribute(0, 3, GL_FLOAT, false, sizeof(float) * 5, 0);
	blitVA->FormatAttribute(1, 2, GL_FLOAT, false, sizeof(float) * 5, (void*)(3 * sizeof(float)));
	blitVA->DisableAttribute(0);
	blitVA->DisableAttribute(1);
	// Create and compile our GLSL program from the shaders
	blitShader = new Shader("res/shaders/SpriteRenderer.shader");
	blitMat = new Material(*blitShader);
}

void SpriteRenderer::Release()
{
	delete blitVB;
	delete blitVA;
	delete blitShader;
	delete blitMat;
}
