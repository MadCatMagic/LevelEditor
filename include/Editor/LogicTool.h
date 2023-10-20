#pragma once
#include "Editor/EditorTool.h"

class LogicTool : public EditorTool
{
public:
	inline LogicTool(Level* t, const std::string& i) : EditorTool(t, i) { };

	static inline void SetInspector(class LogicInspector* i) { inspector = i; }

protected:
	static LogicInspector* inspector;
};

class EntityTool : public LogicTool
{
public:
	EntityTool(Level* t, const std::string& i);

	void OnClick(bool shift, bool ctrl, const v2& exactPos) override;
	void OnHoldClick(bool shift, bool ctrl, const v2& exactPos) override;
	void OnReleaseClick(bool shift, bool ctrl, const v2& exactPos) override;

	void OnGUI() override;

private:
	Entity* toPlace = nullptr;
	Entity* holdingEntity = nullptr;

	std::vector<Entity*> baseEntityTypes;
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