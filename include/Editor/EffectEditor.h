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

	void OnHoldClick(bool shift, bool ctrl, const v2& exactPos) override;

	void OnGUI() override;

	inline TileEffectTool* Init(EffectEditor* p) { parent = p; return this; }
	EffectEditor* parent = nullptr;

private:
	int tileRange = 1;
	// from 1 - 15, with the amount added per tile being (strength - 8)
	int strength = 8;
	float falloff = 0.15f;
};