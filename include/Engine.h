#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h">
#include "imgui_impl_opengl3.h"

#include "Engine/Renderer.h"
#include "Editor/Editor.h"

class Engine
{
public:
	Engine();

	void Mainloop(bool debugging);

	bool CreateWindow(const v2i& windowSize, const std::string& name);

private:
	v2i winSize;
	v2 winProportions;

	GLFWwindow* window = nullptr;

	bool debug = false; 

	void Initialize();
	void Update();
	void Release();

	// aa
	ImGuiIO* io = nullptr;

	Editor editor;
	Level* level = nullptr;
};

