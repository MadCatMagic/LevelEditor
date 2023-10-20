#pragma once
#include "Editor/SpecificEditor.h"
#include "Editor/LogicTool.h"

class LogicEditor;

class LogicInspector
{
public:
	void DrawUI();

	void SetTarget(Entity* target);
	void SetTarget(AreaTrigger* target);

	void* GetTarget() const;
	inline void SetEditor(LogicEditor* e) { editor = e; };
	void SetLevel(Level* l);

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
	friend EntityTool;

	using SpecificEditor::SpecificEditor;
	~LogicEditor();

	void SetupTools() override;
	void Render() override;
	void RenderUI() override;
	void OnReload() override;

	void OnEditorActive() override;
	void OnEditorInactive() override;

	void Update() override;

	void DeleteTrigger(AreaTrigger* t);
	void DeleteEntity(Entity* e);

private:
	LogicInspector inspector;
};