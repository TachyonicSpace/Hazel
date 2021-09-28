#include "hzpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGl/OpenGLTexture.h"

namespace Hazel
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return NewRef<OpenGLTexture2D>(width, height);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	Ref<Texture2D> Texture2D::Create(const std::string& path, bool error)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return NewRef<OpenGLTexture2D>(path, error);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	Ref<Texture2D> Texture2D::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return NewRef<OpenGLTexture2D>();
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}