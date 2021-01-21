
#pragma once

//#include "Hazel/Core/Core.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/MousebuttonCodes.h"

namespace Hazel {

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		static bool IsKeyNotPressed(KeyCode key) { return !IsKeyPressed(key); }

		static bool IsMouseButtonPressed(MouseCode button);
		static bool IsMouseButtonNotPressed(MouseCode button) { return !IsMouseButtonPressed(button); }

		static std::pair<float, float> GetMousePos();
		static float GetMouseX();
		static float GetMouseY();

		static std::pair<float, float> GetViewportPos();
		static float GetViewportMouseX();
		static float GetViewportMouseY();
		static void SetViewportOffset(float x, float y);

	private:
		static float viewportX, viewportY;
	};
}