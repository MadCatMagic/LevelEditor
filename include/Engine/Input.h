#pragma once

// safeguarding against wrong include order
#if !(defined(__gl_h_) || defined(__GL_H__) || defined(_GL_H) || defined(__X_GL_H))
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

struct v2;
struct v2i;

struct ImGuiIO;

namespace Input
{
	// keyboard
	enum class Key {
		BACKSPACE = GLFW_KEY_BACKSPACE, DELETE = GLFW_KEY_DELETE, TAB = GLFW_KEY_TAB, ESCAPE = GLFW_KEY_ESCAPE, SPACE = GLFW_KEY_SPACE,
		UPARROW = GLFW_KEY_UP, DOWNARROW = GLFW_KEY_DOWN, RIGHTARROW = GLFW_KEY_RIGHT, LEFTARROW = GLFW_KEY_LEFT,

		COMMA = GLFW_KEY_COMMA, DOT = GLFW_KEY_PERIOD, GRAVE = GLFW_KEY_GRAVE_ACCENT,

		F1 = GLFW_KEY_F1, F2 = GLFW_KEY_F2, F3 = GLFW_KEY_F3, F4 = GLFW_KEY_F4, F5 = GLFW_KEY_F5, 
		F6 = GLFW_KEY_F6, F7 = GLFW_KEY_F7, F8 = GLFW_KEY_F8, F9 = GLFW_KEY_F9, F10 = GLFW_KEY_F10, 
		F11 = GLFW_KEY_F11, F12 = GLFW_KEY_F12, F13 = GLFW_KEY_F13, F14 = GLFW_KEY_F14, F15 = GLFW_KEY_F15,

		ALPHA0 = GLFW_KEY_0, ALPHA1 = GLFW_KEY_1, ALPHA2 = GLFW_KEY_2, ALPHA3 = GLFW_KEY_3, ALPHA4 = GLFW_KEY_4, 
		ALPHA5 = GLFW_KEY_5, ALPHA6 = GLFW_KEY_6, ALPHA7 = GLFW_KEY_7, ALPHA8 = GLFW_KEY_8, ALPHA9 = GLFW_KEY_9,

		A = GLFW_KEY_A, B = GLFW_KEY_B, C = GLFW_KEY_C, D = GLFW_KEY_D, E = GLFW_KEY_E, F = GLFW_KEY_F, G = GLFW_KEY_G, 
		H = GLFW_KEY_H, I = GLFW_KEY_I, J = GLFW_KEY_J, K = GLFW_KEY_K, L = GLFW_KEY_L, M = GLFW_KEY_M, N = GLFW_KEY_N, 
		O = GLFW_KEY_O, P = GLFW_KEY_P, Q = GLFW_KEY_Q, R = GLFW_KEY_R, S = GLFW_KEY_S, T = GLFW_KEY_T, U = GLFW_KEY_U, 
		V = GLFW_KEY_V, W = GLFW_KEY_W, X = GLFW_KEY_X, Y = GLFW_KEY_Y, Z = GLFW_KEY_Z,

		LSHIFT = GLFW_KEY_LEFT_SHIFT, RSHIFT = GLFW_KEY_RIGHT_SHIFT, LCONTROL = GLFW_KEY_LEFT_CONTROL, RCONTROL = GLFW_KEY_RIGHT_CONTROL, LALT = GLFW_KEY_LEFT_ALT, RALT = GLFW_KEY_RIGHT_ALT
	};

	// help
	const Key validKeys[] {
		Key::BACKSPACE, Key::DELETE, Key::TAB, Key::ESCAPE, Key::SPACE,
		Key::UPARROW, Key::DOWNARROW, Key::RIGHTARROW, Key::LEFTARROW,
		
		Key::COMMA, Key::DOT, Key::GRAVE,

		Key::F1, Key::F2, Key::F3, Key::F4, Key::F5, 
		Key::F6, Key::F7, Key::F8, Key::F9, Key::F10, 
		Key::F11, Key::F12, Key::F13, Key::F14, Key::F15,

		Key::ALPHA0, Key::ALPHA1, Key::ALPHA2, Key::ALPHA3, Key::ALPHA4,
		Key::ALPHA5, Key::ALPHA6, Key::ALPHA7, Key::ALPHA8, Key::ALPHA9,

		Key::A, Key::B, Key::C, Key::D, Key::E, Key::F, Key::G, 
		Key::H, Key::I, Key::J, Key::K, Key::L, Key::M, Key::N, 
		Key::O, Key::P, Key::Q, Key::R, Key::S, Key::T, Key::U, 
		Key::V, Key::W, Key::X, Key::Y, Key::Z,

		Key::LSHIFT, Key::RSHIFT, Key::LCONTROL, Key::RCONTROL, Key::LALT, Key::RALT
	};

	extern bool GetKey(Key key);
	extern bool GetKeyDown(Key key);
	extern bool GetKeyUp(Key key);

	extern bool GetMouseButton(int button);
	extern bool GetMouseButtonDown(int button);
	extern bool GetMouseButtonUp(int button);

	// mouse
	enum class CursorType {
		NORMAL = GLFW_CURSOR_NORMAL, HIDDEN = GLFW_CURSOR_HIDDEN, LOCKED = GLFW_CURSOR_DISABLED
	};

	extern void SetCursorType(CursorType type);
	extern void EnableRawCursorData();
	
	extern float cursorPosX;
	extern float cursorPosY;

	extern v2i GetCursorPixelPos();
	extern v2 GetCursorPos();

	extern float diffX;
	extern float diffY;

	extern float scrollDiff;

	// all
	extern void EnableInput(GLFWwindow* window, ImGuiIO* imguiIO);
	extern void InputUpdate();
};
