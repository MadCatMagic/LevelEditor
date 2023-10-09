#include "Editor/LogicEditor.h"
#include "Editor/EditorGizmos.h"

#include "Compiler/LevelRenderer.h"
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

	entityPlaceTool = new EntityTool(target, "entity_icon.png");
	tools.push_back(entityPlaceTool);
	tools.push_back(new TriggerEditTool(target, "trigger_icon.png"));

	baseEntityTypes.push_back(new Entity());
	baseEntityTypes.push_back(new Camera());
	entityPlaceTool->SetEntityToPlace(baseEntityTypes[0]);
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
	if (Input::GetKeyDown(Input::Key::DELETE))
		inspector.DeleteSelected();

	if (selectedTool == 0)
	{
		ImGui::NewLine();
		static int selectedEntityType = 0;
		if (ImGui::BeginListBox("Entity Type", ImVec2(0.0f, 50.0f)))
		{
			for (int n = 0; n < baseEntityTypes.size(); n++)
			{
				const bool selected = selectedEntityType == n;
				if (ImGui::Selectable(baseEntityTypes[n]->GetType().c_str(), selected))
				{
					selectedEntityType = n;
					entityPlaceTool->SetEntityToPlace(baseEntityTypes[n]);
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}

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
	ImGui::NewLine();
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

void EntityTool::OnClick(bool shift, bool ctrl, const v2& exactPos)
{
	v2 pos = exactPos;

	// try and select an object
	int start = 0;
	bool targetIsEntity = false;
	for (int i = 0; i < level->entities.size(); i++)
		if (level->entities[i] == inspector->GetTarget())
		{
			start = (i + 1) % level->entities.size();
			targetIsEntity = true;
			break;
		}

	if (targetIsEntity && ctrl)
	{
		v4 bounds = inspector->GetBoundsOfSelected();
		if (bounds.x <= pos.x && bounds.y <= pos.y && bounds.z >= pos.x && bounds.w >= pos.y)
		{
			holdingEntity = (Entity*)inspector->GetTarget();
			return;
		}
	}

	bool found = false;
	for (int i = start; i < start + level->entities.size(); i++)
	{
		auto* t = level->entities[i % level->entities.size()];
		v2 bl = t->position - (t->PlaceAtCentreOfTile() ? 0.0f : 0.5f);
		v2 tr = t->position + (t->PlaceAtCentreOfTile() ? 1.0f : 0.5f);
		if (bl.x <= pos.x && pos.x <= tr.x && bl.y <= pos.y && pos.y <= tr.y)
		{
			// set as target normally
			if (!shift && !ctrl)
			{
				inspector->SetTarget(t);
				return;
			}
			// if shift is pressed then set a flag not to create a new entity
			else if (shift && !ctrl)
				found = true;
			else
			{
				holdingEntity = t;
				return;
			}
		}
	}

	// create the entity if so
	if (shift && !found)
	{
		Entity* entity = toPlace->CreateEntityFromName(toPlace->GetType());
		entity->name = toPlace->GetType();
		entity->position = toPlace->PlaceAtCentreOfTile() ? v2i(pos) : v2i(pos + 0.5f);
		level->entities.push_back(entity);
		inspector->SetTarget(level->entities[level->entities.size() - 1]);
	}
}

void EntityTool::OnHoldClick(bool shift, bool ctrl, const v2& exactPos)
{
	if (holdingEntity != nullptr)
	{
		if (!shift)
			holdingEntity->position = holdingEntity->PlaceAtCentreOfTile() ? v2i(exactPos) : v2i(exactPos + 0.5f);
		else
			holdingEntity->position = holdingEntity->PlaceAtCentreOfTile() ? (exactPos - 0.5f) : exactPos;
		holdingEntity->VariableHasChanged();
	}
}

void EntityTool::OnReleaseClick(bool shift, bool ctrl, const v2& exactPos)
{
	holdingEntity = nullptr;
}

void TriggerEditTool::OnClick(bool shift, bool ctrl, const v2i& pos)
{
	if (!shift)
	{
		holding = true;
		startPos = pos;
	}
	else if (shift)
	{
		int start = 0;
		for (int i = 0; i < level->triggers.size(); i++)
			if (level->triggers[i] == inspector->GetTarget())
			{
				start = (i + 1) % level->triggers.size();
				break;
			}

		for (int i = start; i < start + level->triggers.size(); i++)
		{
			auto* t = level->triggers[i % level->triggers.size()];
			if (t->bottomLeft.x <= pos.x && pos.x <= t->topRight.x && t->bottomLeft.y <= pos.y && pos.y <= t->topRight.y)
			{
				inspector->SetTarget(t);
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

		EditorGizmos::DrawRectOutline(a, b, 2.0f);
	}
}

void TriggerEditTool::OnReleaseClick(bool shift, bool ctrl, const v2i& pos)
{
	if (holding)
	{
		AreaTrigger* t = new AreaTrigger();
		t->bottomLeft = v2i(std::min(pos.x, startPos.x), std::min(pos.y, startPos.y));
		t->topRight = v2i(std::max(pos.x, startPos.x), std::max(pos.y, startPos.y));
		t->name = "Trigger";
		level->triggers.push_back(t);
		inspector->SetTarget(level->triggers[level->triggers.size() - 1]);
		holding = false;
	}
}
