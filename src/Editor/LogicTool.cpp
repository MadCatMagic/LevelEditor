#include "Editor/LogicTool.h"
#include "Editor/EditorGizmos.h"

#include "imgui.h"

#include "Editor/LogicEditor.h"

EntityTool::EntityTool(Level* t, const std::string& i)
	: LogicTool(t, i)
{
	baseEntityTypes.push_back(new Entity());
	baseEntityTypes.push_back(new Camera());
	toPlace = baseEntityTypes[0];
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

void EntityTool::OnGUI()
{
	static int selectedEntityType = 0;
	if (ImGui::BeginListBox("Entity Type", ImVec2(0.0f, 50.0f)))
	{
		for (int n = 0; n < baseEntityTypes.size(); n++)
		{
			const bool selected = selectedEntityType == n;
			if (ImGui::Selectable(baseEntityTypes[n]->GetType().c_str(), selected))
			{
				selectedEntityType = n;
				toPlace = baseEntityTypes[n];
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
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
