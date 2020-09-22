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
	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& va, uint32_t indexCount)
	{
		auto count = indexCount ? va->GetIndexBuffer()->GetCount() : indexCount;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}
	void OpenGLRendererAPI::SetViewport(uint32_t xMin, uint32_t yMin, uint32_t xMax, uint32_t yMax)
	{
		glViewport(xMin, yMin, xMax, yMax);
	}

	void OpenGLRendererAPI::DrawPixels(uint32_t width, uint32_t height, float* pixels)
	{
		#if 0
			glDrawPixels(width, height, GL_RGBA, GL_FLOAT, pixels);
		#else
			HZ_CORE_ASSERT(false, "function not implemented yet");
		#endif
	}
}