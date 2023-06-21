#include "Editor/GeometryTool.h"
#include "Engine/Texture/Texture2D.h"
#include "Editor/EditorGizmos.h"
#include <algorithm>

GeometryTool::GeometryTool(Level* target, const std::string& iconName)
{
	level = target;
	sprite = new Texture2D("res/icons/" + iconName);
}

GeometryTool::~GeometryTool()
{
	delete sprite;
}

unsigned int GeometryTool::GetTextureID() const
{
	return sprite->GetID();
}

void DrawGeometryTool::OnHoldClick(bool shift, bool ctrl, const v2i& pos)
{
	TileData* t = level->GetTile(pos);
	if (t != nullptr)
		t->solid = shift;
}

void BoxGeometryTool::OnClick(bool shift, bool ctrl, const v2i& pos)
{
	if (!held)
	{
		startPos = pos;
		solid = shift;
		held = true;
	}
}

void BoxGeometryTool::OnHoldClick(bool shift, bool ctrl, const v2i& pos)
{
	if (held)
	{
		// draw some graphics :)
		EditorGizmos::SetColour(v4(1.0f, 0.0f, 1.0f, 1.0f));

		v2 a = v2i(std::min(pos.x, startPos.x), std::min(pos.y, startPos.y));
		v2 b = v2i(std::max(pos.x, startPos.x), std::max(pos.y, startPos.y)) + v2i::one;

		EditorGizmos::DrawLine(v2(a.x, a.y), v2(a.x, b.y), 2.0f);
		EditorGizmos::DrawLine(v2(a.x, a.y), v2(b.x, a.y), 2.0f);
		EditorGizmos::DrawLine(v2(a.x, b.y), v2(b.x, b.y), 2.0f);
		EditorGizmos::DrawLine(v2(b.x, a.y), v2(b.x, b.y), 2.0f);
	}
}

void BoxGeometryTool::OnReleaseClick(bool shift, bool ctrl, const v2i& pos)
{
	if (held)
	{
		held = false;
		v2i bottomLeft = v2i(std::min(pos.x, startPos.x), std::min(pos.y, startPos.y));
		v2i topRight = v2i(std::max(pos.x, startPos.x), std::max(pos.y, startPos.y));

		for (int i = bottomLeft.x; i <= topRight.x; i++)
			for (int j = bottomLeft.y; j <= topRight.y; j++)
			{
				TileData* t = level->GetTile(v2i(i, j));
				if (t == nullptr)
					continue;

				t->solid = shift;
			}
	}
}
