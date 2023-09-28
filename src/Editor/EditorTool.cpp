#include "Editor/EditorTool.h"
#include "Engine/Texture/Texture2D.h"


EditorTool::EditorTool(Level* target, const std::string& iconName)
{
	level = target;
	sprite = new Texture2D("res/icons/" + iconName);
	sprite->sampling = Texture::Sampling::Nearest;
	sprite->ApplyFiltering();
}

EditorTool::~EditorTool()
{
	delete sprite;
}

unsigned int EditorTool::GetTextureID() const
{
	return sprite->GetID();
}