#include "Editor/LogicEditor.h"
#include "Editor/EditorGizmos.h"

#include "imgui.h"

LogicInspector* LogicTool::inspector = nullptr;

void LogicEditor::SetupTools()
{
	LogicTool::SetInspector(&inspector);

	tools.push_back(new EntityPlaceTool(target, "geometry_draw_icon.png"));
	tools.push_back(new TriggerEditTool(target, "geometry_draw_icon.png"));
	tools.push_back(new TriggerGroupTool(target, "geometry_draw_icon.png"));
}

void LogicEditor::Render()
{
	// first draw entities
	for (const Entity& e : target->entities)
	{
		EditorGizmos::SetColour(e.editorColour);
		EditorGizmos::DrawRect((v2)e.position + v2::one * 0.5f, v2::one * 0.8f);
	}
}

void LogicEditor::RenderUI()
{
	inspector.DrawUI();
}

void LogicEditor::OnReload()
{
}

void LogicInspector::DrawUI()
{
	std::string inspectorTypeName = "/none/";
	if (targetSelected)
		if (entityTarget)
			inspectorTypeName = "Entity";
		else
			inspectorTypeName = "Trigger";
	ImGui::Text(("Inspector - " + inspectorTypeName).c_str());

	if (targetSelected)
	{
		if (ImGui::InputText((inspectorTypeName + " Name").c_str(), nameRaw, 64))
			SetName(nameRaw);

		if (ImGui::ColorEdit4((inspectorTypeName + " Colour").c_str(), &colourRaw.x))
			SetEditorColour(colourRaw);
	}
}

void LogicInspector::SetTarget(Entity* target)
{
	targetIsEntity = true;
	entityTarget = target;
	triggerTarget = nullptr;
	AfterSettingTarget();
}

void LogicInspector::SetTarget(AreaTrigger* target)
{
	targetIsEntity = false;
	entityTarget = nullptr;
	triggerTarget = target;
	AfterSettingTarget();
}

void LogicInspector::AfterSettingTarget()
{
	targetSelected = true;
	memcpy(nameRaw, GetName().c_str(), 64 * sizeof(char));
	colourRaw = GetEditorColour();
}

void LogicInspector::SetName(const std::string& name)
{
	if (targetIsEntity)
		entityTarget->name = name;
	else
		triggerTarget->name = name;
}

std::string LogicInspector::GetName()
{
	if (targetIsEntity)
		return entityTarget->name;
	else
		return triggerTarget->name;
}

void LogicInspector::SetEditorColour(const v4& col)
{
	if (targetIsEntity)
		entityTarget->editorColour = col;
	else
		triggerTarget->editorColour = col;
}

v4 LogicInspector::GetEditorColour()
{
	if (targetIsEntity)
		return entityTarget->editorColour;
	else
		return triggerTarget->editorColour;
}

void EntityPlaceTool::OnClick(bool shift, bool ctrl, const v2i& pos)
{
	bool occupied = false;
	int index = 0;
	for (int i = 0; i < level->entities.size(); i++)
		if (level->entities[i].position == pos)
		{
			occupied = true;
			index = i;
			break;
		}

	if (shift && occupied)
	{
		level->entities.erase(level->entities.begin() + index);
		return;
	}

	if (!shift && !occupied)
	{
		Entity entity;
		entity.name = "Entity";
		entity.position = pos;
		level->entities.push_back(entity);
	}

	if (occupied)
	{
		inspector->SetTarget(&level->entities[index]);
	}
}

void TriggerEditTool::OnClick(bool shift, bool ctrl, const v2i& pos)
{
}

void TriggerGroupTool::OnClick(bool shift, bool ctrl, const v2i& pos)
{
}