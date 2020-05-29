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
		case RendererAPI::API::OpenGL: return newRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		HZ_CORE_ASSERT(false, "unknown API");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::NONE is currently not supported");
		case RendererAPI::API::OpenGL: return newRef<OpenGLShader>(filePath);
		}

		HZ_CORE_ASSERT(false, "unknown API");
		return nullptr;
	}
	void ShaderLibrary::Add(const Ref<Shader>& s)
	{
		auto& name = s->GetName();
		HZ_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Shader Already Exist!");
		m_Shaders[name] = s;
	}
	void ShaderLibrary::Add(const std::string name, const Ref<Shader>& s)
	{
		HZ_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Shader Already Exist!");
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
		HZ_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end(), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exist(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}