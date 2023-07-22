#pragma once
#include "Editor/Editor.h"
#include <vector>

namespace EditorGizmos
{
	extern void Initialize(Editor* e);
	extern void Release();
	
	extern void SetColour(const v4& col);
	extern void DrawLine(const v2& startPos, const v2& endPos, float thickness);

	struct GizmosLine {
		v2 startPos;
		v2 endPos;
		float thickness;
		v4 colour;
	};

	static std::vector<GizmosLine> gizmosToDraw = std::vector<GizmosLine>();

	extern void DrawAllGizmos();
}

