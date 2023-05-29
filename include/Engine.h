#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Engine/Renderer.h"

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
	bool show_demo_window = true;
	bool show_another_window = false;
	v4 clear_color = v4(0.45f, 0.55f, 0.60f, 1.00f);
};

