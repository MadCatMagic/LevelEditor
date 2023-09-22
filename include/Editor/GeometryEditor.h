#pragma once
#include "Editor/SpecificEditor.h"

#include "Compiler/FileManager.h"

class GeometryEditor : public SpecificEditor
{
public:
	using SpecificEditor::SpecificEditor;

	void SetupTools() override;
	void Render() override;
	void RenderUI() override;
	void OnReload() override;

private:
	// testing
	std::vector<ColliderSegment> compiledGeometry;
};