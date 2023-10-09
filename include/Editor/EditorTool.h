#pragma once
#include "Level/Level.h"

class EditorTool
{
public:
	EditorTool(Level* target, const std::string& iconName);
	~EditorTool();

	// special cases, mostly used for logiceditor with entity placement
	inline virtual void OnClick(bool shift, bool ctrl, const v2& exactPos) {};
	inline virtual void OnHoldClick(bool shift, bool ctrl, const v2& exactPos) {};
	inline virtual void OnReleaseClick(bool shift, bool ctrl, const v2& exactPos) {};

	inline virtual void OnClick(bool shift, bool ctrl, const v2i& pos) {};
	inline virtual void OnHoldClick(bool shift, bool ctrl, const v2i& pos) {};
	inline virtual void OnReleaseClick(bool shift, bool ctrl, const v2i& pos) {};

	unsigned int GetTextureID() const;

	inline void SetLevel(Level* l) { level = l; }
	inline void SetLayer(int l) { layer = l; }

protected:
	Level* level;
	int layer = 0;

private:
	class Texture2D* sprite;
};