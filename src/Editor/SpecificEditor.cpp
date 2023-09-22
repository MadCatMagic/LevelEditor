#include "Editor/SpecificEditor.h"
#include "imgui.h"

#include "Editor/Level.h"
#include "Editor/Editor.h"

SpecificEditor::SpecificEditor(class Level* t, class Editor* p)
{
	target = t;
	parent = p;

}

SpecificEditor::~SpecificEditor()
{
	for (EditorTool* tool : tools)
		delete tool;
}