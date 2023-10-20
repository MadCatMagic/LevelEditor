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
		// just trim the chunks now i guess
		selectedEffect->effectMap.tiles->TrimChunks();

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
						val / 15.0f
					));
					EditorGizmos::DrawRect((v2)pos + v2(0.5f), v2::one);
				}

		// for testing, render the outline of active chunks
		EditorGizmos::SetColour(v4(1.0f, 1.0f, 0.0f, 0.2f));
		for each (auto & pair in selectedEffect->effectMap.tiles->map)
			EditorGizmos::DrawRectOutline(pair.first * 16, pair.first * 16 + v2i(16), 2.0f);
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
	static int counter = 0;
	counter++;
	if (counter > 8 - strength)
		counter = 0;
	
	if (counter == 0)
	{
		int correctedStrength = std::max(1, strength - 7);
		for (int yoff = -tileRange / 2; yoff <= tileRange / 2; yoff++)
			for (int xoff = -tileRange / 2; xoff <= tileRange / 2; xoff++)
			{
				if (yoff * yoff + xoff * xoff > tileRange * tileRange)
					continue;
				float fo = 1.0f - std::powf((float)(yoff * yoff + xoff * xoff) / (tileRange * tileRange), falloff);
				int value = parent->selectedEffect->effectMap.tiles->GetTile(pos + v2i(xoff, yoff));
				int offset = correctedStrength * (1 - shift * 2);
				int newv = std::min(std::max(value + (int)(fo * (float)offset), 0), 15);
				parent->selectedEffect->effectMap.tiles->SetTile(pos + v2i(xoff, yoff), newv);
			}
	}
}

#include "imgui.h"
void TileEffectTool::OnGUI()
{
	ImGui::SliderInt("Radius", &tileRange, 1, 5);
	ImGui::SliderInt("Strength", &strength, 1, 15);
	ImGui::SliderFloat("Falloff", &falloff, 0.01f, 1.0f);
}

void GranularEffectTool::OnHoldClick(bool shift, bool ctrl, const v2i& pos)
{
}
