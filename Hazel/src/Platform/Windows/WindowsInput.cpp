#include "hzpch.h"
#include "WindowsInput.h"
#include "Hazel/Application.h"

#include <GLFW/glfw3.h>

namespace Hazel 
{

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keyCode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}


	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}


	double WindowsInput::GetMouseXImpl()
	{
		auto[x, y] = GetMousePosImpl();
		return x;
	}


	double WindowsInput::GetMouseYImpl()
	{
		auto[x, y] = GetMousePosImpl();
		return y;
	}


	std::pair<double, double> WindowsInput::GetMousePosImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return { x, y };
	}


}