#include "Editor/MaterialEditor.h"
#include "Editor/EditorGizmos.h"

void MaterialEditor::SetupTools()
{
	tools.push_back(new MaterialTool(target, "geometry_boxfill_icon.png"));
}

void MaterialEditor::Render()
{
	EditorGizmos::SetColour(v4(1.0f, 1.0f, 0.0f, 0.8f));
	EditorGizmos::DrawRect(v2::one, v2::one * 2);
}

void MaterialEditor::RenderUI()
{
}

void MaterialEditor::OnReload()
{
}
