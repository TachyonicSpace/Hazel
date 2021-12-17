#pragma once

#include "Hazel/Renderer/RendererAPI.h"

namespace Hazel {

	class OpenGLRendererAPI : public RendererAPI
	{

	public:
		void Init(bool blend = false) override;
		virtual void SetViewport(uint32_t xMin, uint32_t yMin, uint32_t xMax, uint32_t yMax) override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& va, uint32_t indexCount = 0, RenderType Primitives = RenderType::TRIANGLES) override;

		virtual void SetLineWidth(float width) override;

		virtual Blending::Types GetSrcFactor() override { return srcFactor; }
		virtual Blending::Types GetDstFactor() override { return dstFactor; }
		virtual Blending::Functions GetBlendFunc() override { return blendFunc; }
		virtual void SetSrcFactor(const Blending::Types& bt) override;
		virtual void SetDstFactor(const Blending::Types& bt) override;
		virtual void SetBlendFunc(const Blending::Functions& bf) override;

		int convertTypesToOpenGLEnum(Blending::Types bt);
		int convertEquationToOpenGLEnum(Blending::Functions bf);
	};

}