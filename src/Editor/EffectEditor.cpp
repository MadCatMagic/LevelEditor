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
	tools[0]->enabled = false;
}

void EffectEditor::Render()
{
	if (selectedEffect == nullptr)
		return;

	// just trim the chunks now i guess
	selectedEffect->effectMap->TrimChunks();

	v2 scalingFac = selectedEffect->tilesPerUnit;
	for each (auto & pair in selectedEffect->effectMap->map)
		for (int yoff = 0; yoff < TILE_CHUNK_SIZE; yoff++)
			for (int xoff = 0; xoff < TILE_CHUNK_SIZE; xoff++)
			{
				v2 pos = v2::Scale(v2((float)xoff, (float)yoff) + pair.first * 16, scalingFac);
				int val = selectedEffect->effectMap->GetTile(pos);
				if (val == 0)
					continue;

				EditorGizmos::SetColour(v4(
					selectedEffect->editorTint.x,
					selectedEffect->editorTint.y,
					selectedEffect->editorTint.z,
					val / 15.0f
				));
				EditorGizmos::DrawRect((v2)pos + scalingFac * 0.5f, scalingFac);
			}

	// for testing, render the outline of active chunks
	EditorGizmos::SetColour(v4(1.0f, 1.0f, 0.0f, 0.2f));
	for each (auto & pair in selectedEffect->effectMap->map)
		EditorGizmos::DrawRectOutline(v2::Scale(scalingFac, (v2)(pair.first * 16)), v2::Scale(scalingFac, (v2)(pair.first * 16 + v2i(16))), 2.0f);
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
		if (selectedEffect != nullptr)
			tools[0]->enabled = true;
	}
}

void EffectEditor::OnReload()
{
}

void TileEffectTool::OnHoldClick(bool shift, bool ctrl, const v2& exactPos)
{
	static int counter = 0;
	static int secondCounter = 0;
	counter++;
	if (counter > 8 - strength)
	{
		counter = 0;
		secondCounter++;
		if (secondCounter == 16)
			secondCounter = 0;
	}
	
	v2 sfac = parent->selectedEffect->tilesPerUnit;
	int correctedStrength = std::max(1, strength - 7);
	for (int yoff = -tileRange; yoff <= tileRange; yoff++)
		for (int xoff = -tileRange; xoff <= tileRange; xoff++)
		{
			if (yoff * yoff + xoff * xoff > tileRange * tileRange)
				continue;
			float fo = 1.0f - std::powf((float)(yoff * yoff + xoff * xoff) / (tileRange * tileRange), falloff);

			if (counter == 0)
			{
				// now use this interior counter combined with the falloff value to work out whether this tile should be updated or not
				int value = parent->selectedEffect->effectMap->GetTile(exactPos + v2::Scale(v2((float)xoff, (float)yoff), sfac));
				int offset = correctedStrength * (1 - shift * 2);
				if (offset * fo < 1.0f)
				{
					float limit = 1.0f / std::abs((float)offset);
					float scale = fo / limit; // 0 <= scale <= 1
					// maybe works?
					if (secondCounter % std::min((int)(1.0f / scale), 16) != 0)
						continue;
				}
				int newv = std::min(std::max(value + (int)std::ceil(fo * (float)offset), 0), 15);
				parent->selectedEffect->effectMap->SetTile(exactPos + v2::Scale(sfac, v2((float)xoff, (float)yoff)), newv);
			}
		}
}

#include "imgui.h"
void TileEffectTool::OnGUI()
{
	ImGui::SliderInt("Radius", &tileRange, 1, 8);
	ImGui::SliderInt("Strength", &strength, 1, 15);
	ImGui::SliderFloat("Falloff", &falloff, 0.01f, 1.0f);
}