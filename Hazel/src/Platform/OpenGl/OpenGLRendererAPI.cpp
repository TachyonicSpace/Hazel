#include "hzpch.h"
#include "OpenGLRendererAPI.h"
#include "Hazel/Core/Application.h"

#include "glad/glad.h"

namespace Hazel {

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         HZ_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       HZ_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_LOW:          HZ_CORE_TRACE(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: HZ_CORE_INFO(message); return;
		}

		HZ_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init(bool blend)
	{
		HZ_PROFILE_FUNCTION();

#ifdef HZ_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif




		if (blend) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		if (!(Application::Get().m_Minimized))
			glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& va, uint32_t indexCount, RenderType Primitives /*= RenderType::GL_TRIANGLES*/)
	{
		va->Bind();
		auto count = indexCount ? indexCount : va->GetIndexBuffer()->GetCount();

		/*
		GL_POINTS
		GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP
		GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN
		*/

		if (!(Application::Get().m_Minimized))
		{
			if (Primitives == RendererAPI::RenderType::LINES || Primitives == RendererAPI::RenderType::LINE_STRIP || Primitives == RendererAPI::RenderType::LINE_LOOP)
				glDrawArrays((GLenum)Primitives, 0, indexCount);
			else
				glDrawElements((GLenum)Primitives, count, GL_UNSIGNED_INT, nullptr);
		}
	}
	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t xMin, uint32_t yMin, uint32_t xMax, uint32_t yMax)
	{
		glViewport(xMin, yMin, xMax, yMax);
	}
}