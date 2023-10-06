#pragma once
#include "Vector.h"
#include <vector>

class Level;

struct Entity
{
	v2 position;
	std::string name;

	v4 editorColour = v4(1.0f, 0.0f, 0.0f, 1.0f);

	inline virtual void UI(Level* l) { };
	inline virtual void Gizmos() { };

	virtual inline std::string GetType() const { return "Entity"; }
	virtual inline bool PlaceAtCentreOfTile() const { return true; }

	virtual inline std::vector<std::string> SaveData() const { return std::vector<std::string>(); }
	virtual inline void LoadData(std::vector<std::string> data) { }

	static Entity* CreateEntityFromName(const std::string& name);

	void inline VariableHasChanged() { editorChangedVariable = true; }

protected:
	// if you use this flag, set it to false afterwards
	bool editorChangedVariable = false;
};

struct Camera : Entity
{
	v2 dimensions = v2::one;
	v2i pixelSize = v2i::one * 16;

	void UI(Level* l) override;
	void Gizmos() override;

	inline std::string GetType() const override { return "Camera"; }
	inline bool PlaceAtCentreOfTile() const { return false; }

	std::vector<std::string> SaveData() const;
	void LoadData(std::vector<std::string> data);

	static void _UpdateRenderer(Camera* targetCam, bool rendering);
	static void _Release();

	static bool cameraShouldUpdatePreview;

private:
	bool preview = false;

	static class SpriteRenderer* renderer;
	static class PixelTexture2D* tex;
};