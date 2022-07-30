#include <GL/glew.h>
#include "Shader.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "Renderer.h"

Shader::Shader(const std::string& filepath)
	:m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(m_FilePath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
	glUniform2f(GetUniformLocation(name), v0, v1);
}
void Shader::SetUniform2f(const std::string& name, float v[2])
{
	glUniform2f(GetUniformLocation(name), v[0], v[1]);
}

void Shader::SetUniform1f(const std::string& name, float v0)
{
	glUniform1f(GetUniformLocation(name), v0);
}

void Shader::SetUniform4ui(const std::string& name, unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3)
{
	glUniform4ui(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform3ui(const std::string& name, unsigned int v0, unsigned int v1, unsigned int v2)
{
	glUniform3ui(GetUniformLocation(name), v0, v1, v2);
}

void Shader::SetUniform2ui(const std::string& name, unsigned int v0, unsigned int v1)
{
	glUniform2ui(GetUniformLocation(name), v0, v1);
}

void Shader::SetUniform1ui(const std::string& name, unsigned int v0)
{
	glUniform1ui(GetUniformLocation(name), v0);
}

void Shader::SetUniform1uiv(const std::string& name, int size, unsigned int v0[])
{
	glUniform1uiv(GetUniformLocation(name), size, v0);
}


unsigned int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocatiobCache.find(name) != m_UniformLocatiobCache.end())
	{
		return m_UniformLocatiobCache[name];
	}
	unsigned int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1) std::cout << "Warning: uniform " << name << " does not exist." << std::endl;
	m_UniformLocatiobCache[name] = location;
	return location;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

ShaderProgramSource Shader::ParseShader(std::string& filePath)
{
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	ShaderType type = ShaderType::NONE;
	std::ifstream stream(filePath);
	std::string line;
	std::stringstream ss[2];

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[int(type)] << line << '\n';
		}
	}
	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}