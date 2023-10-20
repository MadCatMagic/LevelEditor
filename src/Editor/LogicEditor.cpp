#include "Editor/LogicEditor.h"
#include "Editor/EditorGizmos.h"

#include "Engine/Input.h"

#include "imgui.h"

LogicInspector* LogicTool::inspector = nullptr;

LogicEditor::~LogicEditor()
{
	Camera::_Release();
}

void LogicEditor::SetupTools()
{
	LogicTool::SetInspector(&inspector);
	inspector.SetEditor(this);

	tools.push_back(new EntityTool(target, "entity_icon.png"));
	tools.push_back(new TriggerEditTool(target, "trigger_icon.png"));
}

void LogicEditor::Render()
{
	// first draw entities
	for (Entity* e : target->entities)
	{
		EditorGizmos::SetColour(e->editorColour);
		EditorGizmos::DrawRect((v2)e->position + (e->PlaceAtCentreOfTile() ? v2::one * 0.5f : 0.0f), v2::one * 0.8f);
	}

	// then triggers
	for (AreaTrigger* t : target->triggers)
	{
		EditorGizmos::SetColour(v4::Scale(t->editorColour, v4(1.0f, 1.0f, 1.0f, 0.4f)));
		v2 diff = (v2)t->topRight + v2::one - (v2)t->bottomLeft;
		EditorGizmos::DrawRect((v2)t->bottomLeft + diff * 0.5f, diff);
	}

	// then outline selected
	if (inspector.IsAnythingSelected())
	{
		v4 p = inspector.GetBoundsOfSelected();
		EditorGizmos::SetColour(v4(0.8f, 1.0f, 0.2f, 1.0f));
		EditorGizmos::DrawRectOutline(v2(p.x, p.y), v2(p.z, p.w), 3.0f);

		inspector.DrawGizmosOfSelected();
	}
}

void LogicEditor::RenderUI()
{
	inspector.DrawUI();
}

void LogicEditor::OnReload()
{
	inspector.SetLevel(target);
}

void LogicEditor::OnEditorActive()
{
	Camera::cameraShouldUpdatePreview = true;
}

void LogicEditor::OnEditorInactive()
{
	Camera::_UpdateRenderer(nullptr, false);
}

void LogicEditor::Update()
{
	if (Input::GetKeyDown(Input::Key::DELETE))
		inspector.DeleteSelected();
}

void LogicEditor::DeleteTrigger(AreaTrigger* t)
{
	for (int i = 0; i < target->triggers.size(); i++)
		if (target->triggers[i] == t)
		{
			delete target->triggers[i];
			target->triggers.erase(target->triggers.begin() + i);
			break;
		}
}

void LogicEditor::DeleteEntity(Entity* e)
{
	for (int i = 0; i < target->entities.size(); i++)
		if (target->entities[i] == e)
		{
			delete target->entities[i];
			target->entities.erase(target->entities.begin() + i);
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
		if (ImGui::Button("Delete Selected"))
			DeleteSelected();

		if (ImGui::InputText((inspectorTypeName + " Name").c_str(), nameRaw, 64))
			SetName(nameRaw);

		if (ImGui::ColorEdit4((inspectorTypeName + " Colour").c_str(), &colourRaw.x))
			SetEditorColour(colourRaw);

		if (!entityTarget)
		{
			
		}
		else
		{
			if (ImGui::InputFloat2("Position", &entityTarget->position.x))
				entityTarget->VariableHasChanged();
			entityTarget->UI(level);
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

void LogicInspector::SetLevel(Level* l)
{
	level = l;
	entityTarget = nullptr;
	triggerTarget = nullptr;
	targetSelected = false;
}

v4 LogicInspector::GetBoundsOfSelected() const
{
	if (targetIsEntity)
	{
		v2 bl = entityTarget->position - (entityTarget->PlaceAtCentreOfTile() ? 0.0f : 0.5f);
		v2 tr = entityTarget->position + (entityTarget->PlaceAtCentreOfTile() ? 1.0f : 0.5f);
		return v4(bl.x, bl.y, tr.x, tr.y);
	}
	else
		return (v4)v4i(triggerTarget->bottomLeft.x, triggerTarget->bottomLeft.y, triggerTarget->topRight.x + 1, triggerTarget->topRight.y + 1);
}

void LogicInspector::DrawGizmosOfSelected()
{
	if (targetIsEntity)
		entityTarget->Gizmos();
}

void LogicInspector::DeleteSelected()
{
	if (!targetSelected)
		return;

	targetSelected = false;
	if (targetIsEntity)
	{
		editor->DeleteEntity(entityTarget);
		entityTarget = nullptr;
	}
	else
	{
		editor->DeleteTrigger(triggerTarget);
		triggerTarget = nullptr;
	}
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
	{
		entityTarget->name = name;
		entityTarget->VariableHasChanged();
	}
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
	{
		entityTarget->editorColour = col;
		entityTarget->VariableHasChanged();
	}
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