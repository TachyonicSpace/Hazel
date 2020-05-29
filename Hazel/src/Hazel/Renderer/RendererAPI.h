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

	public:
		virtual void Init(bool blend = false) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void SetViewport(uint32_t xMin, uint32_t yMin, uint32_t xMax, uint32_t yMax) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& va) = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};

}