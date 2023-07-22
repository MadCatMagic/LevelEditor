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

	inline void SetLevel(Level* l) { level = l; }
	inline void SetLayer(int l) { layer = l; }

protected:
	void SetSolidity(const v2i& pos, bool isSolid);

	Level* level;
	int layer;

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

class RotateGeometryTool : public GeometryTool
{
public:
	using GeometryTool::GeometryTool;
	
	void OnClick(bool shift, bool ctrl, const v2i& pos) override;
};

class LayerGeometryTool : public GeometryTool
{
public:
	using GeometryTool::GeometryTool;

	void OnClick(bool shift, bool ctrl, const v2i& pos) override;

	inline int* GetCurrentLayer() { return &currentLayer; }

private:
	int currentLayer = 0;
};