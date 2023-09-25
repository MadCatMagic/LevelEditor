#include "Editor/LogicEditor.h"
#include "Editor/EditorGizmos.h"

#include "imgui.h"

LogicInspector* LogicTool::inspector = nullptr;

void LogicEditor::SetupTools()
{
	LogicTool::SetInspector(&inspector);
	inspector.SetEditor(this);

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

	// then triggers
	for (const AreaTrigger& t : target->triggers)
	{
		EditorGizmos::SetColour(t.editorColour * 0.5f);
		v2 diff = t.topRight - t.bottomLeft;
		EditorGizmos::DrawRect((v2)t.bottomLeft + diff * 0.5f, diff);
	}
}

void LogicEditor::RenderUI()
{
	inspector.DrawUI();
}

void LogicEditor::OnReload()
{
}

void LogicEditor::DeleteTrigger(AreaTrigger* t)
{
	for (int i = 0; i < target->triggers.size(); i++)
		if (&target->triggers[i] == t)
		{
			target->triggers.erase(target->triggers.begin() + i);
			break;
		}
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

		if (!entityTarget)
			if (ImGui::Button("Delete Trigger"))
			{
				editor->DeleteTrigger(triggerTarget);
				triggerTarget = nullptr;
				targetSelected = false;
			}
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

void* LogicInspector::GetTarget() const
{
	if (targetIsEntity)
		return entityTarget;
	else
		return triggerTarget;
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
		inspector->SetTarget(&level->entities[level->entities.size() - 1]);
	}

	if (occupied)
	{
		inspector->SetTarget(&level->entities[index]);
	}
}

void TriggerEditTool::OnClick(bool shift, bool ctrl, const v2i& pos)
{
	if (!shift)
	{
		holding = true;
		startPos = pos;
	}
	else
	{
		int start = 0;
		for (int i = 0; i < level->triggers.size(); i++)
			if (&level->triggers[i] == inspector->GetTarget())
			{
				start = (i + 1) % level->triggers.size();
				break;
			}

		for (int i = start; i < start + level->triggers.size(); i++)
		{
			auto &t = level->triggers[i % level->triggers.size()];
			if (t.bottomLeft.x <= pos.x && pos.x <= t.topRight.x && t.bottomLeft.y <= pos.y && pos.y <= t.topRight.y)
			{
				inspector->SetTarget(&level->triggers[i % level->triggers.size()]);
				break;
			}
		}
	}
}

void TriggerEditTool::OnHoldClick(bool shift, bool ctrl, const v2i& pos)
{
	if (holding)
	{
		// draw some graphics :)
		EditorGizmos::SetColour(v4(0.0f, 1.0f, 1.0f, 1.0f));

		v2 a = v2i(std::min(pos.x, startPos.x), std::min(pos.y, startPos.y));
		v2 b = v2i(std::max(pos.x, startPos.x), std::max(pos.y, startPos.y)) + v2i::one;

		EditorGizmos::DrawLine(v2(a.x, a.y), v2(a.x, b.y), 2.0f);
		EditorGizmos::DrawLine(v2(a.x, a.y), v2(b.x, a.y), 2.0f);
		EditorGizmos::DrawLine(v2(a.x, b.y), v2(b.x, b.y), 2.0f);
		EditorGizmos::DrawLine(v2(b.x, a.y), v2(b.x, b.y), 2.0f);
	}
}

void TriggerEditTool::OnReleaseClick(bool shift, bool ctrl, const v2i& pos)
{
	if (holding)
	{
		AreaTrigger t;
		t.bottomLeft = v2i(std::min(pos.x, startPos.x), std::min(pos.y, startPos.y));
		t.topRight = v2i(std::max(pos.x, startPos.x), std::max(pos.y, startPos.y)) + v2i::one;
		t.name = "Trigger";
		level->triggers.push_back(t);
		inspector->SetTarget(&level->triggers[level->triggers.size() - 1]);
		holding = false;
	}
}

void TriggerGroupTool::OnClick(bool shift, bool ctrl, const v2i& pos)
{
}