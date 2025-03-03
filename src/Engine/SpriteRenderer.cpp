#include "Engine/SpriteRenderer.h"
#include "Engine/Renderer.h"

#include "Engine/Mesh/Material.h"
#include "Engine/Mesh/Shader.h"
#include "Engine/Mesh/VertexArray.h"
#include "Engine/Mesh/VertexBuffer.h"

#include <iostream>

#include "tracy/Tracy.hpp"

// render stuff, xy are positions, zw are tex coords
static const float blitQuadData[24] = {
	-1.0f, -1.0f, 0.0f, 1.0f,
	 1.0f, -1.0f, 1.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 0.0f,
	-1.0f,  1.0f, 0.0f, 0.0f,
	 1.0f, -1.0f, 1.0f, 1.0f,
	 1.0f,  1.0f, 1.0f, 0.0f,
};

static VertexBuffer* blitVB;
static VertexArray* blitVA;
static VertexBuffer* blitInstancingVB;
static Shader* blitShader;
static Material* blitMat;

std::unordered_map<int, std::vector<SpriteRenderer::InstanceData>, SpriteRenderer::CustomHash> SpriteRenderer::instancedData = std::unordered_map<int, std::vector<SpriteRenderer::InstanceData>, SpriteRenderer::CustomHash>();
//std::unordered_map<int, int, SpriteRenderer::CustomHash> SpriteRenderer::numInstancesPerTex = std::unordered_map<int, int, SpriteRenderer::CustomHash>();

// things to be drawn first are at the front with the highest layer value
std::vector<SpriteRenderer*> SpriteRenderer::renderers = std::vector<SpriteRenderer*>();

v2i SpriteRenderer::pixelScreenSize = v2i(1, 1);

SpriteRenderer::SpriteRenderer(int drawlevel)
	: tint(v4::one), render(true)
{
	layer = drawlevel;
	renderers.push_back(this);

	// need to ensure it is inserted at the correct level
	int i = (int)renderers.size() - 1;
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
	Clear();

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
	//numInstancesPerTex[tex->GetID()] += 1;

	tex2D = nullptr;
	rt = tex;
	pixelSize = v2i(tex->width, tex->height);
}

void SpriteRenderer::Clear() 
{
	//if (tex2D != nullptr || rt != nullptr)
	//	numInstancesPerTex[GetTextureID()] -= 1;

	tex2D = nullptr;
	rt = nullptr;
	pixelSize = v2i();
}

void SpriteRenderer::Render()
{
	if (!render || (tex2D == nullptr && rt == nullptr))
		return;

	unsigned int src = GetTextureID();
	v2i dimensions = GetTextureSize();
	
	v2 realscale = v2::Scale(v2((float)dimensions.x / (float)pixelScreenSize.x, (float)dimensions.y / (float)pixelScreenSize.y), scale);

	// target position
	// same as Editor::PixelToScreen
	v2 realpos = realpos = v2::Scale(pos + v2::Scale((v2)pixelSize * 0.5f, scale), v2::Reciprocal(pixelScreenSize)) * 2.0f - v2::one;

	InstanceData obj = InstanceData(realpos, realscale, rotation, tint);

	instancedData[src].push_back(obj);
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

unsigned int SpriteRenderer::GetTextureID() const
{
	if (rt != nullptr)
		return rt->colourBuffer->GetID();
	else if (tex2D != nullptr)
		return tex2D->GetID();
	return -1;
}

v2i SpriteRenderer::GetTextureSize() const
{
	if (rt != nullptr)
		return v2i(rt->width, rt->height);
	else if (tex2D != nullptr)
		return v2i(tex2D->width, tex2D->height);
	return v2i::zero;
}

void SpriteRenderer::RenderAll(RenderTexture* dest)
{
	ZoneScoped;

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
	// Set our "renderedTexture" sampler to use Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	blitMat->SetTexture("blitTexture", 0);

	blitVB->Bind();
	blitVA->Bind();

	int currentIndex = 0;
	while (currentIndex < (int)renderers.size())
		currentIndex = RenderLayer(currentIndex);
}

int SpriteRenderer::RenderLayer(int startingIndex)
{
	ZoneScoped;

	int i = CollectInstances(startingIndex);

	for each (const std::pair<int, std::vector<InstanceData>>& pair in instancedData)
	{
		if (pair.second.size() == 0)
			continue;

		glBindTexture(GL_TEXTURE_2D, pair.first);
		blitInstancingVB->SetData((const void*)pair.second.data(), sizeof(InstanceData) * (unsigned int)pair.second.size(), VertexBuffer::UsageHint::StaticDraw);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, (GLsizei)pair.second.size());

		instancedData[pair.first].clear();
	}

	return i;
}

int SpriteRenderer::CollectInstances(int startingIndex)
{
	ZoneScoped;

	int i = startingIndex;
	int currentLayer = renderers[i]->layer;
	while (i < (int)renderers.size() && currentLayer == renderers[i]->layer)
	{
		renderers[i]->Render();
		i++;
	}
	return i;
}

void SpriteRenderer::Initialise()
{
	// setup blit stuff
	blitVB = new VertexBuffer(blitQuadData, sizeof(float) * 24);
	blitInstancingVB = new VertexBuffer(nullptr, 0);
	blitVA = new VertexArray();
	blitVA->Construct();
	// basic data
	blitVA->EnableAttribute(0);
	blitVA->EnableAttribute(1);
	blitVB->Bind();
	blitVA->FormatAttribute(0, 2, GL_FLOAT, false, sizeof(float) * 4, 0);
	blitVA->FormatAttribute(1, 2, GL_FLOAT, false, sizeof(float) * 4, (void*)(2 * sizeof(float)));
	// instanced stuff
	blitVA->EnableAttribute(2);
	blitVA->EnableAttribute(3);
	blitVA->EnableAttribute(4);
	blitInstancingVB->Bind();
	blitVA->FormatAttribute(2, 2, GL_FLOAT, false, sizeof(float) * 9, 0);							// (pos.x, pos.y)
	blitVA->FormatAttribute(3, 3, GL_FLOAT, false, sizeof(float) * 9, (void*)(2 * sizeof(float)));	// (scale.x, scale.y, rotation)
	blitVA->FormatAttribute(4, 4, GL_FLOAT, false, sizeof(float) * 9, (void*)(5 * sizeof(float)));	// (tint.r, tint.g, tint.b, tint.a)
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	
	// Create and compile our GLSL program from the shaders
	blitShader = new Shader("res/shaders/SpriteRenderer.shader");
	blitMat = new Material(blitShader);
}

void SpriteRenderer::Release()
{
	delete blitVB;
	delete blitVA;
	delete blitInstancingVB;
	delete blitShader;
	delete blitMat;
}
