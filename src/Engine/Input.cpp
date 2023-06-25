#include "Engine/Input.h"
#include "Vector.h"
#include <unordered_map>
#include <iostream>

#include "imgui.h"

namespace Input
{
	GLFWwindow* window = nullptr;
	ImGuiIO* io = nullptr;
	v2i winSize;

	// stores the opengl states
	std::unordered_map<Key, int> keystate;
	// stores the actual states: 0 = not pressed, 1 = pressed, 2 = pressed this frame, 3 = unpressed this frame
	std::unordered_map<Key, int> keys;

	std::unordered_map<int, int> mouseButtonState;
	std::unordered_map<int, int> mouseButtons;

	static void CreateKeyMaps()
	{
		keystate = std::unordered_map<Key, int>();
		keys = std::unordered_map<Key, int>();
		mouseButtonState = std::unordered_map<int, int>();
		mouseButtons = std::unordered_map<int, int>();
		for (Key key : validKeys)
		{
			keystate[key] = 0;
			keys[key] = 0;
		}
		for (int i = 0; i < 3; i++)
		{
			mouseButtonState[i] = 0;
			mouseButtons[i] = 0;
		}
	}

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		// if a valid key in glfw
		if (key != GLFW_KEY_UNKNOWN)
			// loop through all valid keys to see whether this key matches any of them
			for (Key k : validKeys)
				if (k == (Key)key)
				{
					// if so then set and return
					keystate[(Key)key] = action;
					return;
				}
	}

	bool GetKey(Key key)
	{
		return keys[key] == 1 || keys[key] == 2;
	}

	bool GetKeyDown(Key key)
	{
		return keys[key] == 2;
	}

	bool GetKeyUp(Key key)
	{
		return keys[key] == 3;
	}

	bool GetMouseButton(int button)
	{
		return mouseButtons[button] == 1 || mouseButtons[button] == 2;
	}

	bool GetMouseButtonDown(int button)
	{
		return mouseButtons[button] == 2;
	}

	bool GetMouseButtonUp(int button)
	{
		return mouseButtons[button] == 3;
	}

	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (io != nullptr && io->WantCaptureMouse)
			return;
		mouseButtonState[button] = action;
	}

	float cursorPosX = 0.0f;
	float cursorPosY = 0.0f;

	static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
	{
		cursorPosX = (float)xpos;
		cursorPosY = (float)ypos;
	}

	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		scrollDiff = (float)yoffset;
	}

	void SetCursorType(CursorType type)
	{
		glfwSetInputMode(window, GLFW_CURSOR, (int)type);
	}

	void EnableRawCursorData()
	{
		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	v2i GetCursorPixelPos()
	{
		return v2i((int)cursorPosX, (int)cursorPosY);
	}

	v2 GetCursorPos()
	{
		return v2(cursorPosX / winSize.x, cursorPosY / winSize.y);
	}

	void EnableInput(GLFWwindow* _window, ImGuiIO* imguiIO)
	{
		io = imguiIO;
		window = _window;

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		winSize = v2i(width, height);

		glfwSetKeyCallback(window, KeyCallback);
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwSetCursorPosCallback(window, CursorPositionCallback);
		glfwSetScrollCallback(window, ScrollCallback);

		EnableRawCursorData();
		CreateKeyMaps();
	}

	float previousCursorPosX = -1.0f;
	float previousCursorPosY = -1.0f;
	float diffX = 0.0f;
	float diffY = 0.0f;
	// always integer
	float scrollDiff = 0.0f;

	void InputUpdate()
	{
		// cursor difference to last frame
		if (previousCursorPosX != -1.0)
		{
			diffX = cursorPosX - previousCursorPosX;
			diffY = cursorPosY - previousCursorPosY;
		}
		previousCursorPosX = cursorPosX;
		previousCursorPosY = cursorPosY;

		// update keyboard states
		for (std::pair<Key, int> key : keystate)
		{
			// 0 : not pressed
			// 1 : pressed
			// 2 : pressed this frame
			// 3 : unpressed this frame

			// not pressed this frame and not pressed or unpressed the last frame means not pressed
			if (key.second == 0 && (keys[key.first] == 0 || keys[key.first] == 3))
			{
				keys[key.first] = 0;
			}
			// pressed this frame and pressed or newly pressed last frame means pressed
			else if ((key.second == 1 || key.second == 2) && (keys[key.first] == 1 || keys[key.first] == 2))
			{
				keys[key.first] = 1;
			}
			// pressed this frame and not pressed or unpressed last frame means newly pressed
			else if ((key.second == 1 || key.second == 2) && (keys[key.first] == 0 || keys[key.first] == 3))
			{
				keys[key.first] = 2;
			}
			// unpressed this frame and pressed or newly pressed last frame means unpressed
			else if (key.second == 0 && (keys[key.first] == 1 || keys[key.first] == 2))
			{
				keys[key.first] = 3;
			}
		}

		// so much stuff
		for (std::pair<int, int> button : mouseButtonState)
		{
			// not pressed this frame and not pressed or unpressed the last frame means not pressed
			if (button.second == 0 && (mouseButtons[button.first] == 0 || mouseButtons[button.first] == 3))
			{
				mouseButtons[button.first] = 0;
			}
			// pressed this frame and pressed or newly pressed last frame means pressed
			else if ((button.second == 1 || button.second == 2) && (mouseButtons[button.first] == 1 || mouseButtons[button.first] == 2))
			{
				mouseButtons[button.first] = 1;
			}
			// pressed this frame and not pressed or unpressed last frame means newly pressed
			else if ((button.second == 1 || button.second == 2) && (mouseButtons[button.first] == 0 || mouseButtons[button.first] == 3))
			{
				mouseButtons[button.first] = 2;
			}
			// unpressed this frame and pressed or newly pressed last frame means unpressed
			else if (button.second == 0 && (mouseButtons[button.first] == 1 || mouseButtons[button.first] == 2))
			{
				mouseButtons[button.first] = 3;
			}
		}
	}
}