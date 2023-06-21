#pragma once
#include "Editor/Editor.h"

namespace EditorGizmos
{
	extern void Initialize(Editor* e);
	extern void Release();
	
	extern void SetColour(const v4& col);
	extern void DrawLine(const v2& startPos, const v2& endPos, float thickness);
}

