#pragma once
#include "Editor/SpecificEditor.h"

class LogicEditor;

class LogicInspector
{
public:
	void DrawUI();

	void SetTarget(Entity* target);
	void SetTarget(AreaTrigger* target);

	void* GetTarget() const;
	inline void SetEditor(LogicEditor* e) { editor = e; };
	inline void SetLevel(Level* l) { level = l; }

	v4 GetBoundsOfSelected() const;
	inline bool IsAnythingSelected() const { return targetSelected; }
	void DrawGizmosOfSelected();

	void DeleteSelected();

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

	LogicEditor* editor;
	Level* level;
};


class LogicEditor : public SpecificEditor
{
public:
	using SpecificEditor::SpecificEditor;
	~LogicEditor();

	void SetupTools() override;
	void Render() override;
	void RenderUI() override;
	void OnReload() override;

	void OnEditorActive() override;
	void OnEditorInactive() override;

	void DeleteTrigger(AreaTrigger* t);
	void DeleteEntity(Entity* e);

private:
	class EntityPlaceTool* entityPlaceTool = nullptr;

	LogicInspector inspector;

	std::vector<Entity*> baseEntityTypes;
};

class LogicTool : public EditorTool
{
public:
	inline LogicTool(Level* t, const std::string& i) : EditorTool(t, i) { };

	static inline void SetInspector(LogicInspector* i) { inspector = i; }

protected:
	static LogicInspector* inspector;
};

class EntityPlaceTool : public LogicTool
{
public:
	using LogicTool::LogicTool;

	void OnClick(bool shift, bool ctrl, const v2& exactPos) override;

	inline void SetEntityToPlace(Entity* e) { toPlace = e; }

private:
	Entity* toPlace = nullptr;
};

class TriggerEditTool : public LogicTool
{
public:
	using LogicTool::LogicTool;

	void OnClick(bool shift, bool ctrl, const v2i& pos) override;
	void OnHoldClick(bool shift, bool ctrl, const v2i& pos) override;
	void OnReleaseClick(bool shift, bool ctrl, const v2i& pos) override;

private:
	bool holding = false;
	v2i startPos;
};