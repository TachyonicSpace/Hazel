#pragma once

#include <glm/glm.hpp>

#include "Hazel/Renderer/Shader.h"


//namespace Hazel {
//
//	//a struct to allow us to return multiple return types
//	struct ShaderProgramSource {
//		std::string VertexSource;
//		std::string FragmentSource;
//	};
//
//	class Shader
//	{
//	private:
//		std::string m_FilePath;
//		unsigned int m_RendererID;
//		std::unordered_map<std::string, int> m_UniformLocationCache;
//
//
//		int GetUniformLocation(const std::string& name);
//		ShaderProgramSource parseShader(const std::string& filepath);
//		unsigned int CompileShader(unsigned int type, const std::string source);
//		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
//	public:
//		Shader(const std::string& filePath);
//		~Shader();
//
//		void Bind() const;
//		void UnBind() const;
//
//		//set uniforms
//		void SetUniform1i(const std::string& name, int value);
//		void SetUniform1f(const std::string& name, float value);
//		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
//		void SetUniformMat4f(const std::string& name, const glm::mat4& mat);
//	};
//
//}

namespace Hazel {

	//a struct to allow us to return multiple return types
	struct ShaderProgramSource {
		std::string VertexSource;
		std::string FragmentSource;
	};

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();

		void Bind() const override;
		void UnBind() const override;

		void UploadUniformInt(const std::string& name, const int& val);

		void UploadUniformFloat(const std::string& name, const float& val);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& vec);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& vec);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& vec);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private: 
		ShaderProgramSource parseShader(const std::string& filepath);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	private:
		uint32_t m_RendererID;
		std::string m_FilePath;
	};

}