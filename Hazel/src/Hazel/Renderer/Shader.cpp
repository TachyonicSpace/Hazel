#include "hzpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGl/OpenGLShader.h"

namespace Hazel {

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported");
		case RendererAPI::API::OpenGL: return NewRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		HZ_CORE_ASSERT(false, "unknown API");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported");
		case RendererAPI::API::OpenGL: return NewRef<OpenGLShader>(filePath);
		}

		HZ_CORE_ASSERT(false, "unknown API");
		return nullptr;
	}
	void ShaderLibrary::Add(const Ref<Shader>& s)
	{
		auto& name = s->GetName();
		Add(name, s);
	}
	void ShaderLibrary::Add(const std::string name, const Ref<Shader>& s)
	{
		HZ_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = s;
	}
	Hazel::Ref<Hazel::Shader> ShaderLibrary::Load(const std::string& filePath)
	{
		auto shader = Shader::Create(filePath);
		Add(shader);
		return shader;
	}
	Hazel::Ref<Hazel::Shader> ShaderLibrary::Load(const std::string& name, const std::string& filePath)
	{
		auto shader = Shader::Create(filePath);
		Add(name, shader);
		return shader;
	}

	Hazel::Ref<Hazel::Shader> ShaderLibrary::Get(const std::string& name)
	{
		HZ_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}