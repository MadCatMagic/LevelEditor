#include "Editor/EditorTool.h"
#include "Engine/Texture/Texture2D.h"


EditorTool::EditorTool(Level* target, const std::string& iconName)
{
	level = target;
	sprite = new Texture2D("res/icons/" + iconName);
}

EditorTool::~EditorTool()
{
	delete sprite;
}

unsigned int EditorTool::GetTextureID() const
{
	return sprite->GetID();
}

void LayerTool::OnClick(bool shift, bool ctrl, const v2i& pos)
{
	if (shift)
		currentLayer--;
	else
		currentLayer++;

	if (currentLayer < 0)
		currentLayer = 0;
	else if (currentLayer >= TILE_CHUNK_LAYERS)
		currentLayer = TILE_CHUNK_LAYERS - 1;
}
