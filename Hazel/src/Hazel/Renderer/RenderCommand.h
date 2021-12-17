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

		inline static void DrawIndexed(const Ref<VertexArray>& va, uint32_t indexCount = 0, RenderType PrimativeType = RenderType::TRIANGLES)
		{
			s_RendererAPI->DrawIndexed(va, indexCount, PrimativeType);
		}

		static void SetLineWidth(float width)
		{
			s_RendererAPI->SetLineWidth(width);
		}

		static Blending::Types GetSrcFactor() { return s_RendererAPI->GetSrcFactor(); }
		static Blending::Types GetDstFactor() { return s_RendererAPI->GetDstFactor(); }
		static Blending::Functions GetBlendFunc() { return s_RendererAPI->GetBlendFunc(); }
		
		static void SetSrcFactor(const Blending::Types& bt) { s_RendererAPI->SetSrcFactor(bt); }
		static void SetDstFactor(const Blending::Types& bt) { s_RendererAPI->SetDstFactor(bt); }
		static void SetBlendFunc(const Blending::Functions& bf) { s_RendererAPI->SetBlendFunc(bf); }


	private:
		static Scope<RendererAPI> s_RendererAPI;
	};

}