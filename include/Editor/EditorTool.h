#pragma once
#include "Editor/Level.h"

class EditorTool
{
public:
	EditorTool(Level* target, const std::string& iconName);
	~EditorTool();

	virtual inline void OnClick(bool shift, bool ctrl, const v2i& pos) {};
	virtual inline void OnHoldClick(bool shift, bool ctrl, const v2i& pos) {};
	virtual inline void OnReleaseClick(bool shift, bool ctrl, const v2i& pos) {};

	unsigned int GetTextureID() const;

	inline void SetLevel(Level* l) { level = l; }
	inline void SetLayer(int l) { layer = l; }

protected:

	Level* level;
	int layer = 0;

private:
	class Texture2D* sprite;
};

class LayerTool : public EditorTool
{
public:
	using EditorTool::EditorTool;

	void OnClick(bool shift, bool ctrl, const v2i& pos) override;

	inline int* GetCurrentLayer() { return &currentLayer; }

private:
	int currentLayer = 0;
};
