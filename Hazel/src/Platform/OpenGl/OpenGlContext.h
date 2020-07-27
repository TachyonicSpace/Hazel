#pragma once
#include "Hazel/Renderer/GraphicsContext.h"
//#include "Hazel/g

struct GLFWwindow;

namespace Hazel {


	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* window);

		void Init() override;
		void SwapBuffers() override;
	private:
		GLFWwindow* m_Window;
	};

}