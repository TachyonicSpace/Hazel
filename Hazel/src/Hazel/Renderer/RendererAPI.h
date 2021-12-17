#pragma once

#include "glm/glm.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "VertexArray.h"

#include "RenderSettingsEnums.h"

namespace Hazel {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL
		};

	public:
		virtual ~RendererAPI() = default;

		virtual void Init(bool blend = false) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void SetViewport(uint32_t xMin, uint32_t yMin, uint32_t xMax, uint32_t yMax) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& va, uint32_t indexCount = 0, RenderType Primitives = RenderType::TRIANGLES) = 0;

		virtual Blending::Types GetSrcFactor() = 0;
		virtual Blending::Types GetDstFactor() = 0;
		virtual Blending::Functions GetBlendFunc() = 0;

		virtual void SetSrcFactor(const Blending::Types& bt) = 0;
		virtual void SetDstFactor(const Blending::Types& bt) = 0;
		virtual void SetBlendFunc(const Blending::Functions& bf) = 0;

		virtual void SetLineWidth(float width) = 0;

		inline static API GetAPI() { return s_API; }
		static Scope<RendererAPI> Create();
	private:
		static API s_API;
	protected:
		Blending::Types srcFactor, dstFactor;
		Blending::Functions blendFunc;
	};

}


