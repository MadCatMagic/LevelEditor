#pragma once
#include "Editor/SpecificEditor.h"

#include "Compiler/FileManager.h"

#include "Engine/PixelTexture2D.h"
#include "Engine/SpriteRenderer.h"

class GeometryEditor : public SpecificEditor
{
public:
	using SpecificEditor::SpecificEditor;
	~GeometryEditor();

	void SetupTools() override;
	void Render() override;
	void RenderUI() override;
	void OnReload() override;

private:
	// testing
	std::vector<ColliderSegment> compiledGeometry;

	PixelTexture2D* t = nullptr;
	class SpriteRenderer* rend = nullptr;
};