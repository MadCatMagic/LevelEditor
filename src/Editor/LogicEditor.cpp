#include "Editor/LogicEditor.h"
#include "Editor/EditorGizmos.h"

#include "Engine/SpriteRenderer.h"
#include "Engine/PixelTexture2D.h"
#include "Compiler/LevelRenderer.h"

#include "imgui.h"

LogicInspector* LogicTool::inspector = nullptr;

LogicEditor::~LogicEditor()
{
	if (tex != nullptr)
		delete tex;
	if (renderer != nullptr)
		delete renderer;
}

void LogicEditor::SetupTools()
{
	LogicTool::SetInspector(&inspector);
	inspector.SetEditor(this);

	entityPlaceTool = new EntityPlaceTool(target, "geometry_draw_icon.png");
	tools.push_back(entityPlaceTool);
	tools.push_back(new TriggerEditTool(target, "geometry_draw_icon.png"));

	baseEntityTypes.push_back(new Entity());
	baseEntityTypes.push_back(new Camera());
	entityPlaceTool->SetEntityToPlace(baseEntityTypes[0]);

	renderer = new SpriteRenderer(-10000);
	renderer->pixelScreenSize = v2i(640, 480);
}

void LogicEditor::Render()
{
	// first draw entities
	for (Entity* e : target->entities)
	{
		EditorGizmos::SetColour(e->editorColour);
		EditorGizmos::DrawRect((v2)e->position + v2::one * 0.5f, v2::one * 0.8f);
	}

	// then triggers
	for (AreaTrigger* t : target->triggers)
	{
		EditorGizmos::SetColour(t->editorColour * 0.5f);
		v2 diff = (v2)t->topRight + v2::one - (v2)t->bottomLeft;
		EditorGizmos::DrawRect((v2)t->bottomLeft + diff * 0.5f, diff);
	}

	// then outline selected
	if (inspector.IsAnythingSelected())
	{
		v4 p = inspector.GetBoundsOfSelected();
		EditorGizmos::SetColour(v4(0.8f, 1.0f, 0.2f, 1.0f));
		EditorGizmos::DrawRectOutline(v2(p.x, p.y), v2(p.z, p.w), 4.0f);

		inspector.DrawGizmosOfSelected();
	}

	if (tex != nullptr)
	{
		renderer->SetTexture(tex->GetTexture());
	}
}

void LogicEditor::RenderUI()
{
	static v2i cc = v2i(20, 20);
	ImGui::InputInt2("camera centre", &cc.x);

	if (ImGui::Button("test renderer"))
	{
		LevelRenderer r = LevelRenderer(target);
		Camera c;
		c.position = cc;
		c.dimensions = v2(40, 30);
		c.pixelSize = v2i(640, 480);
		if (tex != nullptr)
			delete tex;
		tex = r.RenderCamera(c);
	}

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
		if (ImGui::InputText((inspectorTypeName + " Name").c_str(), nameRaw, 64))
			SetName(nameRaw);

		if (ImGui::ColorEdit4((inspectorTypeName + " Colour").c_str(), &colourRaw.x))
			SetEditorColour(colourRaw);

		if (!entityTarget)
		{
			if (ImGui::Button("Delete Trigger"))
			{
				editor->DeleteTrigger(triggerTarget);
				triggerTarget = nullptr;
				targetSelected = false;
			}
		}
		else
		{
			ImGui::InputFloat2("Position", &entityTarget->position.x);
			entityTarget->UI();
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

v4 LogicInspector::GetBoundsOfSelected() const
{
	if (targetIsEntity)
		return v4(entityTarget->position.x, entityTarget->position.y, entityTarget->position.x + 1.0f, entityTarget->position.y + 1.0f);
	else
		return v4(triggerTarget->bottomLeft.x, triggerTarget->bottomLeft.y, triggerTarget->topRight.x + 1.0f, triggerTarget->topRight.y + 1.0f);
}

void LogicInspector::DrawGizmosOfSelected()
{
	if (targetIsEntity)
		entityTarget->Gizmos();
}

void LogicInspector::DeleteEntity(Entity* e)
{
	if (targetIsEntity && e == entityTarget)
		targetSelected = false;
	entityTarget = nullptr;
	editor->DeleteEntity(e);
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
		if (level->entities[i]->position == pos)
		{
			occupied = true;
			index = i;
			break;
		}

	if (shift && occupied)
	{
		inspector->DeleteEntity(level->entities[index]);
		return;
	}

	if (!shift && !occupied)
	{
		Entity* entity = toPlace->CreateEntity();
		entity->name = toPlace->GetType();
		entity->position = pos;
		level->entities.push_back(entity);
		inspector->SetTarget(level->entities[level->entities.size() - 1]);
	}

	if (occupied)
	{
		inspector->SetTarget(level->entities[index]);
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
			if (&level->triggers[i] == inspector->GetTarget())
			{
				start = (i + 1) % level->triggers.size();
				break;
			}

		for (int i = start; i < start + level->triggers.size(); i++)
		{
			auto &t = level->triggers[i % level->triggers.size()];
			if (t->bottomLeft.x <= pos.x && pos.x <= t->topRight.x && t->bottomLeft.y <= pos.y && pos.y <= t->topRight.y)
			{
				inspector->SetTarget(level->triggers[i % level->triggers.size()]);
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