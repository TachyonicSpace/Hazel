#pragma once

#include "glm/glm.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "VertexArray.h"

namespace Hazel {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL
		};
		enum class RenderType
		{
			GL_POINTS = 0x0000,
			GL_LINES, GL_LINE_LOOP, GL_LINE_STRIP,
			GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN
		};

	public:
		virtual ~RendererAPI() = default;

		virtual void Init(bool blend = false) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void SetViewport(uint32_t xMin, uint32_t yMin, uint32_t xMax, uint32_t yMax) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& va, uint32_t indexCount = 0, RenderType Primitives = RenderType::GL_TRIANGLES) = 0;

		inline static API GetAPI() { return s_API; }
		static Scope<RendererAPI> Create();
	private:
		static API s_API;
	};

}