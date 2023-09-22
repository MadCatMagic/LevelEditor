#pragma once
#include "Editor/SpecificEditor.h"

class MaterialEditor : public SpecificEditor
{
public:
	using SpecificEditor::SpecificEditor;

	void SetupTools() override;
	void Render() override;
	void RenderUI() override;
	void OnReload() override;

private:

};

class MaterialTool : public EditorTool
{
public:
	using EditorTool::EditorTool;
};