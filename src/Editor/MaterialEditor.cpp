#include "Editor/MaterialEditor.h"
#include "Editor/EditorGizmos.h"

#include "imgui.h"

MaterialEditor::MaterialEditor(Level* t, Editor* p)
	: SpecificEditor(t, p)
{
	mats = new MaterialManager();
	mats->PopulateMaterialList();
}

MaterialEditor::~MaterialEditor()
{
	if (mats != nullptr)
		delete mats;
}

void MaterialEditor::SetupTools()
{
	MaterialTool* t = new MaterialTool(target, "geometry_boxfill_icon.png");
	t->SetToolReference(&selectedTool);
	tools.push_back(t);
	
}

void MaterialEditor::Render()
{
	for (int x = 0; x < target->dimensions.x; x++)
		for (int y = 0; y < target->dimensions.y; y++)
		{
			TileData* t = target->GetTile(v2i(x, y), parent->GetCurrentLayer());

			if (t == nullptr || !t->solid)
				continue;

			v4 col = v4(mats->GetMaterialColour(t->material));
			EditorGizmos::SetColour(col);
			EditorGizmos::DrawRect(v2(x, y) + v2::one * 0.5f, v2::one * 0.5f);
		}
}

void MaterialEditor::RenderUI()
{
	static int currentTab = 0;
	if (ImGui::Button("Previous Tab <--"))
		currentTab = (currentTab + 15) % mats->GetNumPages(); // negative % positive = negative which is wrong
	ImGui::SameLine();
	if (ImGui::Button("--> Next Tab"))
		currentTab = (currentTab + 1) % mats->GetNumPages();

	if (ImGui::BeginListBox(mats->GetPageName(currentTab).c_str()))
	{
		for (int n = 0; n < mats->GetPageLength(currentTab); n++)
		{
			const bool isSelected = (mats->GetSelectedOnPage(currentTab) == n);
			if (ImGui::Selectable(mats->GetMaterialName(currentTab * 16 + n).c_str(), isSelected))
				mats->SetSelectedOnPage(currentTab, n);

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	selectedTool = currentTab * 16 + mats->GetSelectedOnPage(currentTab);
}

void MaterialEditor::OnReload()
{
}

void MaterialTool::OnHoldClick(bool shift, bool ctrl, const v2i& pos)
{
	TileData* t = level->GetTile(pos, layer);
	if (t != nullptr && t->solid)
	{
		t->material = *selectedTool;
	}
}
