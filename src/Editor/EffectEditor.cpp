#include "Editor/EffectEditor.h"
#include "Level/Effects.h"
#include "Editor/EditorGizmos.h"

#include "imgui.h"

EffectEditor::~EffectEditor()
{
	if (effects != nullptr)
		delete effects;
}

void EffectEditor::SetupTools()
{
	effects = new EffectManager();

	tools.push_back((new TileEffectTool(target, "geometry_draw_icon.png"))->Init(this));
	tools.push_back((new GranularEffectTool(target, "geometry_boxfill_icon.png"))->Init(this));
	tools[0]->enabled = false;
	tools[1]->enabled = false;
}

void EffectEditor::Render()
{
	if (selectedEffect == nullptr)
		return;

	if (selectedEffect->perTile)
	{
		for each (auto & pair in selectedEffect->effectMap.tiles->map)
			for (int yoff = 0; yoff < TILE_CHUNK_SIZE; yoff++)
				for (int xoff = 0; xoff < TILE_CHUNK_SIZE; xoff++)
				{
					v2i pos = v2i(xoff, yoff) + pair.first * 16;
					int val = selectedEffect->effectMap.tiles->GetTile(pos);
					if (val == 0)
						continue;

					EditorGizmos::SetColour(v4(
						selectedEffect->editorTint.x,
						selectedEffect->editorTint.y,
						selectedEffect->editorTint.z,
						val * (1.0f / 15.0f)
					));
					EditorGizmos::DrawRect((v2)pos + v2(0.5f), v2::one);
				}
	}
	else
	{
		// todo
	}
}

void EffectEditor::RenderUI()
{
	static int currentSelection = 0;
	if (ImGui::BeginListBox("Effects"))
	{
		for (int n = 0; n < (int)effects->GetNumEffects(); n++)
		{
			const bool isSelected = currentSelection == n;
			if (ImGui::Selectable(effects->GetEffect(n)->name.c_str(), isSelected))
				currentSelection = n;

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	// update effect
	if (effects->GetNumEffects() > 0)
	{
		selectedEffect = effects->GetEffect(currentSelection);
		// make sure to set what is enabled
		tools[0]->enabled = selectedEffect->perTile;
		tools[1]->enabled = !selectedEffect->perTile;
	}
}

void EffectEditor::OnReload()
{
}

void TileEffectTool::OnHoldClick(bool shift, bool ctrl, const v2i& pos)
{
	int value = parent->selectedEffect->effectMap.tiles->GetTile(pos);
	if (value != -1)
		parent->selectedEffect->effectMap.tiles->SetTile(pos, value + 1 - shift * 2);
}

void GranularEffectTool::OnHoldClick(bool shift, bool ctrl, const v2i& pos)
{
}
