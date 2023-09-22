#pragma once

#include <vector>
#include "Editor/EditorTool.h"


class SpecificEditor
{
public:
	SpecificEditor(class Level* t, class Editor* p);
	~SpecificEditor();

	// just adding them to the tools vector, 
	// all the actual tool programming is done for you in Editor
	virtual inline void SetupTools() {};

	// for either custom rendering of sprites as an overlay or for gizmos
	virtual inline void Render() {};
	// tools are automatically rendered, do not put them in here
	virtual inline void RenderUI() {};

	// called when the scene is reloaded
	virtual inline void OnReload() {};

	friend class Editor;

protected:
	std::vector<EditorTool*> tools;

	Level* target = nullptr;
	Editor* parent = nullptr;
};
