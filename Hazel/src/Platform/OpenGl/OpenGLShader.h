#pragma once

#include "Hazel/Renderer/Shader.h"


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
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader();

		void Bind() const override;
		void UnBind() const override;

		virtual const std::string& GetName() const override { return m_Name; }

		void UploadUniformInt(const std::string& name, const int& val);
		void UploadUniformIntArray(const std::string& name, int* vals, uint32_t count);

		void UploadUniformFloat(const std::string& name, const float& val);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& vec);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& vec);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& vec);

		void UploadUniformFloat2(const std::string& name, const float*& vec);
		void UploadUniformFloat3(const std::string& name, const float*& vec);
		void UploadUniformFloat4(const std::string& name, const float*& vec);

		void UploadUniformFloat4(const std::string& name, const Color& vec);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		ShaderProgramSource parseShader(const std::string& filepath);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};

}