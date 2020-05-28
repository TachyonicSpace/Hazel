#pragma once
#include "Hazel/Renderer/GraphicsContext.h"
//#include "Hazel/g

struct GLFWwindow;

namespace Hazel {


	class OpenGlContext : public GraphicsContext
	{
	public:
		OpenGlContext(GLFWwindow* window);

		void Init() override;
		void SwapBuffers() override;
	private:
		GLFWwindow* m_Window;
	};

}