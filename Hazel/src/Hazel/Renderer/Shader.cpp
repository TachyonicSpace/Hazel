#include "hzpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGl/OpenGLShader.h"

namespace Hazel {

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported");
		case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		HZ_CORE_ASSERT(false, "unknown API");
		return nullptr;
	}
}