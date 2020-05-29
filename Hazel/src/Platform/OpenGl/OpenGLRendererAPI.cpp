#include "hzpch.h"
#include "OpenGLRendererAPI.h"

#include "glad/glad.h"

namespace Hazel {

	void OpenGLRendererAPI::Init(bool blend)
	{
		if (blend) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& va)
	{
		glDrawElements(GL_TRIANGLES, va->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
	void OpenGLRendererAPI::SetViewport(uint32_t xMin, uint32_t yMin, uint32_t xMax, uint32_t yMax)
	{
		glViewport(xMin, yMin, xMax, yMax);
	}
}