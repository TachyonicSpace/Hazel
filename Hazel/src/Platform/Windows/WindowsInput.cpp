#include "hzpch.h"
#include "Hazel/Core/input.h"
#include "Hazel/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Hazel {

	float Input::viewportX = -1;
	float Input::viewportY = -1;

	bool Input::IsKeyPressed(KeyCode key)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::GetMousePos()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePos();
		return x;
	}

	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePos();
		return y;
	}


	std::pair<float, float> Hazel::Input::GetViewportPos()
	{
		return { viewportX, viewportY };
	}
	float Hazel::Input::GetViewportMouseX()
	{
		return GetMouseX() - viewportX;
	}
	float Hazel::Input::GetViewportMouseY()
	{
		return GetMouseY() - viewportY;
	}
	void Hazel::Input::SetViewportOffset(float x, float y)
	{
		int winX, winY;

		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		glfwGetWindowPos(window, &winX, &winY);

		viewportX = x - winX;
		viewportY = y - winY;
	}

}