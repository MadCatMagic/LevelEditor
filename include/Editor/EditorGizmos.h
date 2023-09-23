#pragma once
#include "Editor/Editor.h"
#include <vector>

namespace EditorGizmos
{
	extern void Initialize(Editor* e);
	extern void Release();
	
	extern void SetColour(const v4& col);
	extern void DrawLine(const v2& startPos, const v2& endPos, float thickness);

	extern void DrawRect(const v2& centre, const v2& size);

	enum GizmosType { Line, Rect };

	struct GizmosObject {
		v4 colour;
		v4 p1;
		v4 p2;

		GizmosType type;
	};

	static std::vector<GizmosObject> gizmosToDraw = std::vector<GizmosObject>();

	extern void DrawAllGizmos();
}

