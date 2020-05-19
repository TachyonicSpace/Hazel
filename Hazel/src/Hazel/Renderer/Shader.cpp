#include "hzpch.h"
#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "glad/glad.h"


//namespace Hazel {
//
//    Shader::Shader(const std::string& filePath)
//        :m_FilePath(filePath), m_RendererID(0)
//    {
//        //makes some shader code from the file at this location
//        ShaderProgramSource source = parseShader(filePath);
//        //compiles the shader and returns the location on the gpu
//        m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
//
//    }
//
//    Shader::~Shader()
//    {
//        glDeleteProgram(m_RendererID);
//    }
//
//    //allows us to parse in shader from files instread of string literals
//    ShaderProgramSource Shader::parseShader(const std::string& filepath) {
//        std::ifstream stream(filepath);
//
//        enum class ShaderType {
//            NONE = -1, VERTEX = 0, FRAGMENT = 1
//        };
//
//        std::string line;
//        std::stringstream ss[2];
//        ShaderType type = ShaderType::NONE;
//        while (getline(stream, line)) {
//            if (line.find("#shader") != std::string::npos) {
//                if (line.find("vertex") != std::string::npos) {
//                    type = ShaderType::VERTEX;
//                }
//                else if (line.find("fragment") != std::string::npos) {
//                    type = ShaderType::FRAGMENT;
//                }
//            }
//            else {
//                ss[(int)type] << line << "\n";
//            }
//        }return { ss[0].str(), ss[1].str() };
//    }
//
//    /*compile a shader (gpu program) from a string*/
//    unsigned int Shader::CompileShader(unsigned int type, const std::string source) {
//        //creates a shader of specified type
//        unsigned int id = glCreateShader(type);
//        //stores the source code as a string
//        const char* src = source.c_str();
//        //load source code into openGL and compiles it
//        glShaderSource(id, 1, &src, nullptr);
//        glCompileShader(id);
//
//        //makes sure code compiled correctly
//        int result;
//        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
//        //if compile error, print our the error
//        if (result == GL_FALSE) {
//            int length;
//            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
//            char* message = (char*)alloca(length * sizeof(char));
//            glGetShaderInfoLog(id, length, &length, message);
//            std::cout << "failed to compile \n" << message << "\n";
//            glDeleteShader(id);
//            return 0;
//        }
//        //else return compiled code location
//        return id;
//    }
//
//    //create the shaders from strings
//    unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
//        //makes a new gpu program, and both shaders compiled code
//        unsigned int program = glCreateProgram();
//        unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
//        unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
//
//        //attatch shaders to program and makes sure it is valid
//        glAttachShader(program, vs);
//        glAttachShader(program, fs);
//        glLinkProgram(program);
//        glValidateProgram(program);
//
//        //delete the shaders since they are incorporated into the program and return program
//        glDeleteShader(vs);
//        glDeleteShader(fs);
//
//        return(program);
//    }
//
//
//    void Shader::Bind() const
//    {
//        //tells openGL to use this shader
//        glUseProgram(m_RendererID);
//    }
//
//    void Shader::UnBind() const
//    {
//        //tells openGL to use this shader
//        glUseProgram(0);
//    }
//
//    void Shader::SetUniform1i(const std::string& name, int value)
//    {
//        glUniform1i(GetUniformLocation(name), value);
//    }
//
//    void Shader::SetUniform1f(const std::string& name, float value)
//    {
//        glUniform1f(GetUniformLocation(name), value);
//    }
//
//    void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
//    {
//
//        glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
//    }
//
//    void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& mat)
//    {
//        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
//
//    }
//
//    int Shader::GetUniformLocation(const std::string& name)
//    {
//        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
//            return m_UniformLocationCache[name];
//
//        int location = glGetUniformLocation(m_RendererID, name.c_str());
//        if (location == -1)
//            std::cout << "warning: uniform '" << name << "' doesnt exist";
//
//        m_UniformLocationCache[name] = location;
//        return location;
//    }
//
//}

namespace Hazel
{


	Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
			// Create an empty vertex shader handle
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		//checks to make sure compilation succeeded
		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			HZ_CORE_ERROR("{0}", infoLog.data());
			HZ_CORE_ASSERT(false, "Vertex shader compilation failed");

			// In this simple program, we'll just leave
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar*)fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			HZ_CORE_ERROR("{0}", infoLog.data());
			HZ_CORE_ASSERT(false, "fragment shader compilation failed");

			// In this simple program, we'll just leave
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_RendererId = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(m_RendererId, vertexShader);
		glAttachShader(m_RendererId, fragmentShader);

		// Link our program
		glLinkProgram(m_RendererId);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_RendererId, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererId, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererId, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_RendererId);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			HZ_CORE_ERROR("{0}", infoLog.data());
			HZ_CORE_ASSERT(false, "shader linking failed");

			// In this simple program, we'll just leave
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_RendererId, vertexShader);
		glDetachShader(m_RendererId, fragmentShader);
	}


	Shader::~Shader()
	{
		glDeleteProgram(m_RendererId);
	}


	void Shader::Bind() const
	{
		glUseProgram(m_RendererId);
	}


	void Shader::UnBind() const
	{
		glUseProgram(0);
	}

	void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint loc = glGetUniformLocation(m_RendererId, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, &matrix[0][0]);
	}


}