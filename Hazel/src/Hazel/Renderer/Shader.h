#pragma once

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <unordered_map>

namespace Hazel {

	class Shader
	{
	public:
		~Shader() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Ref<Shader> Create(const std::string& filePath);

		virtual void UploadUniformInt(const std::string& name, const int& val) = 0;

		virtual void UploadUniformFloat(const std::string& name, const float& val) = 0;
		virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& vec) = 0;
		virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& vec) = 0;
		virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& vec) = 0;

		virtual void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) = 0;
		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) = 0;
	};

	class ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& s);
		void Add(const std::string name, const Ref<Shader>& s);
		Ref<Shader> Load(const std::string& filePath);
		Ref<Shader> Load(const std::string& name, const std::string& filePath);

		Ref<Shader> Get(const std::string& name);

		bool Exist(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};

}