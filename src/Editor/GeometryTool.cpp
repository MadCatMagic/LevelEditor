#include "Editor/GeometryTool.h"
#include "Editor/EditorGizmos.h"
#include <algorithm>

void GeometryTool::SetSolidity(const v2i& pos, bool isSolid)
{
	TileData* t = level->GetTile(pos, layer);
	if (t != nullptr)
	{
		if (t->solid != isSolid)
			t->slant = 0;
		t->solid = isSolid;
	}
}

void DrawGeometryTool::OnHoldClick(bool shift, bool ctrl, const v2i& pos)
{
	SetSolidity(pos, shift);
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

		EditorGizmos::DrawRectOutline(a, b, 2.0f);
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
				SetSolidity(v2i(i, j), solid);
			}
	}
}

void RotateGeometryTool::OnClick(bool shift, bool ctrl, const v2i& pos)
{
	TileData* t = level->GetTile(pos, layer);
	if (t != nullptr && t->solid)
	{
		// credit to monty for this disgusting hardcoding
		// it works and i hope never to look at it again.
		const int lookupTable[] {
			0, 1, 2, 2, 3, 0, 3, 0, 4, 1, 0, 0, 4, 0, 0, 0
		};

		const int permutationTable[][5]{
			{1, 2, 3, 4, 0},
			{1, 2, 3, 4, 0},
			{2, 3, 4, 1, 0},
			{3, 4, 1, 2, 0},
			{4, 1, 2, 3, 0}
		};

		// 1001 is first slant value
		// 1100 is second
		// 0110
		// 0011
		// etc... for one neighbour too.
		unsigned int lookupValue =
			IsSolid(pos + v2i(1, 0)) +
			(IsSolid(pos + v2i(0, 1)) << 1) +
			(IsSolid(pos + v2i(-1, 0)) << 2) +
			(IsSolid(pos + v2i(0, -1)) << 3);

		int offset = lookupTable[lookupValue];
		for (int i = 0; i < 5; i++)
		{
			if (permutationTable[offset][i] == t->slant)
			{
				t->slant = permutationTable[offset][(i + 1 - 2 * shift) % 5];
				return;
			}
		}
	}
}

bool RotateGeometryTool::IsSolid(const v2i& pos)
{
	TileData* t = level->GetTile(pos, layer);
	return t != nullptr && t->solid;
}