#pragma once
#include "Editor/SpecificEditor.h"
#include "Level/LevelMaterial.h"

class MaterialEditor : public SpecificEditor
{
public:
	MaterialEditor(class Level* t, class Editor* p);
	~MaterialEditor();

	void SetupTools() override;
	void Render() override;
	void RenderUI() override;
	void OnReload() override;

private:
	MaterialManager* mats;

	int selectedTool = 0;
};

class MaterialTool : public EditorTool
{
public:
	using EditorTool::EditorTool;

	void OnHoldClick(bool shift, bool ctrl, const v2i& pos) override;

	inline void SetToolReference(int* tool) { selectedTool = tool; }

private:
	int* selectedTool = nullptr;
};