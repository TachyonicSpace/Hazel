#include "hzpch.h"
#include "OpenGlContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Hazel {
	OpenGlContext::OpenGlContext(GLFWwindow* window)
		:m_Window(window)
	{
		HZ_CORE_ASSERT(window, "Window is null")
	}
	void OpenGlContext::Init()
	{
		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HZ_CORE_ASSERT(status, "failed to initialize glad!");
	}


	void OpenGlContext::SwapBuffers()
	{
		glfwSwapBuffers(m_Window);
	}


}