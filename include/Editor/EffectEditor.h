#pragma once
#include "SpecificEditor.h"

class EffectEditor : public SpecificEditor
{
public:
	friend class TileEffectTool;
	friend class GranularEffectTool;

	using SpecificEditor::SpecificEditor;
	~EffectEditor();

	void SetupTools() override;
	void Render() override;
	void RenderUI() override;
	void OnReload() override;

private:
	class EffectManager* effects = nullptr;
	struct Effect* selectedEffect = nullptr;
};

class TileEffectTool : public EditorTool
{
public:
	using EditorTool::EditorTool;

	void OnHoldClick(bool shift, bool ctrl, const v2i& pos) override;

	inline TileEffectTool* Init(EffectEditor* p) { parent = p; return this; }
	EffectEditor* parent = nullptr;
};

class GranularEffectTool : public EditorTool
{
public:
	using EditorTool::EditorTool;

	void OnHoldClick(bool shift, bool ctrl, const v2i& pos) override;

	inline GranularEffectTool* Init(EffectEditor* p) { parent = p; return this; }
	EffectEditor* parent = nullptr;
};