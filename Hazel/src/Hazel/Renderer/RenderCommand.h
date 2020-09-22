#pragma once

#include "RendererAPI.h"

namespace Hazel {

	class RenderCommand
	{
	public:
		inline static void Init(bool blend = false)
		{
			s_RendererAPI->Init(blend);
		}

		inline static void SetViewport(uint32_t xMin, int yMin, uint32_t xMax, uint32_t yMax)
		{
			s_RendererAPI->SetViewport(xMin, yMin, xMax, yMax);
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& va, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexed(va, indexCount);
		}

		static void DrawPixels(uint32_t width, uint32_t height, float* pixels)
		{
			s_RendererAPI->DrawPixels(width, height, pixels);
		}
	private:
		static Scope<RendererAPI> s_RendererAPI;
	};

}