#pragma once
#include "Editor/SpecificEditor.h"



class LogicInspector
{
public:
	void DrawUI();

	void SetTarget(Entity* target);
	void SetTarget(AreaTrigger* target);

private:
	void AfterSettingTarget();

	void SetName(const std::string& name);
	std::string GetName();

	void SetEditorColour(const v4& col);
	v4 GetEditorColour();

	char nameRaw[64] = "";
	v4 colourRaw;

	bool targetSelected = false;
	bool targetIsEntity = true;
	Entity* entityTarget = nullptr;
	AreaTrigger* triggerTarget = nullptr;
};


class LogicEditor : public SpecificEditor
{
public:
	using SpecificEditor::SpecificEditor;

	void SetupTools() override;
	void Render() override;
	void RenderUI() override;
	void OnReload() override;

private:
	LogicInspector inspector;
};

class LogicTool : public EditorTool
{
public:
	inline LogicTool(Level* t, const std::string& i) : EditorTool(t, i) { };

	static inline void SetInspector(LogicInspector* i) { inspector = i; }

protected:
	static LogicInspector* inspector;
};

// todo
class EntityPlaceTool : public LogicTool
{
public:
	using LogicTool::LogicTool;

	void OnClick(bool shift, bool ctrl, const v2i& pos) override;
};

// todo
class TriggerEditTool : public LogicTool
{
public:
	using LogicTool::LogicTool;

	void OnClick(bool shift, bool ctrl, const v2i& pos) override;
};

// todo
class TriggerGroupTool : public LogicTool
{
public:
	using LogicTool::LogicTool;

	void OnClick(bool shift, bool ctrl, const v2i& pos) override;
};