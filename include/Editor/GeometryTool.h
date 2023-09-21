#pragma once
#include "Editor/EditorTool.h"

class GeometryTool : public EditorTool
{
public:
	inline GeometryTool(Level* t, const std::string& i) : EditorTool(t, i) { };

protected:
	void SetSolidity(const v2i& pos, bool isSolid);
};


class DrawGeometryTool : public GeometryTool
{
public:
	using GeometryTool::GeometryTool;

	void OnHoldClick(bool shift, bool ctrl, const v2i& pos) override;
};

class BoxGeometryTool : public GeometryTool
{
public:
	using GeometryTool::GeometryTool;

	void OnClick(bool shift, bool ctrl, const v2i& pos) override;
	void OnHoldClick(bool shift, bool ctrl, const v2i& pos) override;
	void OnReleaseClick(bool shift, bool ctrl, const v2i& pos) override;

private:
	v2i startPos; 
	bool held = false;
	bool solid;
};

class RotateGeometryTool : public GeometryTool
{
public:
	using GeometryTool::GeometryTool;
	
	void OnClick(bool shift, bool ctrl, const v2i& pos) override;
};