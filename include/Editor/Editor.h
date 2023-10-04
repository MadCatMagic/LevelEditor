#pragma once
#include "Level/Level.h"
#include <vector>
#include "SpecificEditor.h"

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

	inline v2i GetWinSize() const { return winSize; }
	inline int GetCurrentLayer() const { return currentLayer; }

private:
	enum EditorMode {Geometry = 0, Material = 1, Logic = 2};
	EditorMode mode = EditorMode::Geometry;

	int GetIndex(const v2i& pos) const;
	int GetIndex(int x, int y, int layer) const;

	void ReloadLevel(Level* l);

	void ChangeEditor(EditorMode newMode);

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
	std::vector<SpecificEditor*> editors;
	EditorTool* selectedTool = nullptr;

	class LayerTool* layerTool = nullptr;
	int currentLayer = 0;

};

