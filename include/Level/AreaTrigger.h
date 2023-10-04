#pragma once
#include "Vector.h"

struct AreaTrigger
{
	v2i bottomLeft;
	v2i topRight;

	int groupId = -1;
	std::string name;

	v4 editorColour = v4(1.0f, 0.0f, 1.0f, 1.0f);
};