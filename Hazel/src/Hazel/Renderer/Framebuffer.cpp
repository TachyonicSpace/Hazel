#include "hzpch.h"
#include "Framebuffer.h"


#include "Renderer.h"
#include "Platform/OpenGl/OpenGLFramebuffer.h"

namespace Hazel
{

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecs& specs)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported");
		case RendererAPI::API::OpenGL: return newRef<OpenGLFrameBuffer>(specs);
		}

		HZ_CORE_ASSERT(false, "unknown API");
		return nullptr;
	}

}