#include "hzpch.h"
#include "OpenGLShader.h"
//#include <iostream>
#include <fstream>
//#include <string>
//#include <sstream>

#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>


namespace Hazel
{

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		HZ_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		HZ_PROFILE_FUNCTION();

		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		HZ_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		HZ_PROFILE_FUNCTION();

		glDeleteProgram(m_RendererID);
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		HZ_PROFILE_FUNCTION();

		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				HZ_CORE_ERROR("Could not read from file '{0}'", filepath);
			}
		}
		else
		{
			HZ_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		HZ_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			HZ_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			HZ_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		HZ_PROFILE_FUNCTION();

		GLuint program = glCreateProgram();
		HZ_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				HZ_CORE_ERROR("{0}", infoLog.data());
				HZ_CORE_ASSERT(false, "Shader compilation failure!");
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		m_RendererID = program;

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);

			for (auto id : glShaderIDs)
				glDeleteShader(id);

			HZ_CORE_ERROR("{0}", infoLog.data());
			HZ_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}
	}


    void OpenGLShader::Bind() const
	{
		HZ_PROFILE_FUNCTION();

        glUseProgram(m_RendererID);
    }

    void OpenGLShader::UnBind() const
	{
		HZ_PROFILE_FUNCTION();

        glUseProgram(0);
    }

    void OpenGLShader::UploadUniformInt(const std::string& name, const int& val)
	{
		HZ_PROFILE_FUNCTION();

        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(loc, val);
    }
    void OpenGLShader::UploadUniformIntArray(const std::string& name, int* vals, uint32_t count)
    {
		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1iv(loc, count, vals);
    }
    void OpenGLShader::UploadUniformFloat(const std::string& name, const float& val)
	{
		HZ_PROFILE_FUNCTION();

        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(loc, val);
    }
    void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& vec)
	{
		HZ_PROFILE_FUNCTION();

        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(loc, vec.x, vec.y);
    }
	void OpenGLShader::UploadUniformFloat2(const std::string& name, const float*& vec)
	{
		HZ_PROFILE_FUNCTION();

		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(loc, vec[0], vec[1]);
	}
	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& vec)
	{
		HZ_PROFILE_FUNCTION();

        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(loc, vec.x, vec.y, vec.z);
    }

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const float*& vec)
	{
		HZ_PROFILE_FUNCTION();

		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(loc, vec[0], vec[1], vec[2]);
	}
	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& vec)
	{
		HZ_PROFILE_FUNCTION();

        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
    }

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const float*& vec)
	{
		HZ_PROFILE_FUNCTION();

		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(loc, vec[0], vec[1], vec[2], vec[3]);
	}
    void OpenGLShader::UploadUniformFloat4(const std::string& name, const Color& vec)
    {
		HZ_PROFILE_FUNCTION();

		GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(loc, vec.GetRGBA()[0], vec.GetRGBA()[1], vec.GetRGBA()[2], vec.GetRGBA()[3]);
    }
	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		HZ_PROFILE_FUNCTION();

        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix3fv(loc, 1, GL_FALSE, &matrix[0][0]);
    }

    void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		HZ_PROFILE_FUNCTION();

        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, &matrix[0][0]);
    }
}