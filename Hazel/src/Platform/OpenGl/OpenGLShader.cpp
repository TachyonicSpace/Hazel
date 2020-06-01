#include "hzpch.h"
#include "hzpch.h"
#include "OpenGLShader.h"
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

    OpenGLShader::OpenGLShader(const std::string& filePath)
        :m_RendererID(-1)
	{
		HZ_PROFILE_FUNCTION();

        //makes some shader code from the file at this location
        ShaderProgramSource source = parseShader(filePath);
        //compiles the shader and returns the location on the gpu
        m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);

        // takes the name out of the filePath
        auto lastSlash = filePath.find_last_of("/\\");
        lastSlash = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;

		auto lastDot = filePath.rfind('.');
		auto count = (lastDot == std::string::npos) ? filePath.size() - lastSlash: lastDot - lastSlash;

        m_Name = filePath.substr(lastSlash, count);
    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
        :m_Name(name), m_RendererID(-1)
	{
		HZ_PROFILE_FUNCTION();

        m_RendererID = CreateShader(vertexSrc, fragmentSrc);
    }


    OpenGLShader::~OpenGLShader()
	{
		HZ_PROFILE_FUNCTION();

        glDeleteProgram(m_RendererID);
    }


    //allows us to parse in shader from files instread of string literals
	ShaderProgramSource OpenGLShader::parseShader(const std::string& filepath) 
    {
		HZ_PROFILE_FUNCTION();

        std::ifstream stream(filepath);

        enum class ShaderType {
            NONE = -1, VERTEX = 0, FRAGMENT = 1
        };

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;
        while (getline(stream, line)) {
            if (line.find("#type") != std::string::npos) {
                if (line.find("vertex") != std::string::npos) {
                    type = ShaderType::VERTEX;
                }
                else if (line.find("fragment") != std::string::npos || line.find("pixel") != std::string::npos) {
                    type = ShaderType::FRAGMENT;
                }
            }
            else {
                ss[(int)type] << line << "\n";
            }
        }return { ss[0].str(), ss[1].str() };
    }

    /*compile a shader (gpu program) from a string*/
    unsigned int OpenGLShader::CompileShader(unsigned int type, const std::string& source) 
	{
		HZ_PROFILE_FUNCTION();

        //creates a shader of specified type
        unsigned int id = glCreateShader(type);
        //stores the source code as a string
        const char* src = source.c_str();
        //load source code into openGL and compiles it
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        //makes sure code compiled correctly
        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        //if compile error, print our the error
        if (result == GL_FALSE) {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char* message = (char*)alloca(length * sizeof(char));
            glGetShaderInfoLog(id, length, &length, message);
            std::cout << "failed to compile \n" << message << "\n";
            glDeleteShader(id);
            return 0;
        }

        m_RendererID = id;
        //else return compiled code location
        return id;
    }

    //create the shaders from strings
    unsigned int OpenGLShader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) 
	{
		HZ_PROFILE_FUNCTION();

        //makes a new gpu program, and both shaders compiled code
        unsigned int program = glCreateProgram();
        unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
        unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

        //attatch shaders to program and makes sure it is valid
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glValidateProgram(program);

        //delete the shaders since they are incorporated into the program and return program
        glDeleteShader(vs);
        glDeleteShader(fs);

        return(program);
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
    void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& vec)
	{
		HZ_PROFILE_FUNCTION();

        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(loc, vec.x, vec.y, vec.z);
    }

    void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& vec)
	{
		HZ_PROFILE_FUNCTION();

        GLint loc = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
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