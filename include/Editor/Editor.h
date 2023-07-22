#pragma once
#include "Level.h"
#include <vector>
#include "GeometryTool.h"

#include "Compiler/FileManager.h"

class Editor
{
public:
	Editor();
	~Editor();

	void Initialize(Level* target, const v2i& windowSize);
	void Update();

	void Render(class RenderTexture* target);
	void RenderUI(struct ImGuiIO* io);

	v2 PixelToWorld(const v2& p) const;
	v2 WorldToPixel(const v2& p) const;

	v2 PixelToScreen(const v2& p) const;
	v2 ScreenToPixel(const v2& p) const;

private:
	int GetIndex(const v2i& pos) const;
	int GetIndex(int x, int y, int layer) const;

	void ReloadLevel(Level* l);

	Level* level{ nullptr };
	class Texture2D* whiteTex = nullptr;
	Texture2D* slantTex = nullptr;
	class SpriteRenderer* spriteRenderers = nullptr;

	// viewing information/rendering
	v2 viewerPosition;
	v2 viewerScale = v2::one;
	int zoomLevel = 7;

	v2i winSize;
	bool initialized = false;

	// editing tools
	std::vector<GeometryTool*> tools;
	int selectedTool = 0;

	int* currentLayer = nullptr;
	
	// testing
	std::vector<ColliderSegment> compiledGeometry;
};

