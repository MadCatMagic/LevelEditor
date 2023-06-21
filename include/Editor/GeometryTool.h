#pragma once
#include "Editor/Level.h"
#include <vector>

class GeometryTool
{
public:
	GeometryTool(Level* target, const std::string& iconName);
	~GeometryTool();

	virtual inline void OnClick(bool shift, bool ctrl, const v2i& pos) {};
	virtual inline void OnHoldClick(bool shift, bool ctrl, const v2i& pos) {};
	virtual inline void OnReleaseClick(bool shift, bool ctrl, const v2i& pos) {};

	unsigned int GetTextureID() const;

protected:
	Level* level;

private:
	class Texture2D* sprite;
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