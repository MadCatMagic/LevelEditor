#pragma once
#include "Editor/SpecificEditor.h"
#include "Editor/LevelMaterial.h"

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
};

class MaterialTool : public EditorTool
{
public:
	using EditorTool::EditorTool;

	MaterialManager* mats;
};