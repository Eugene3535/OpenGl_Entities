#pragma once

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderProgram
{
public:
	ShaderProgram() : id(glCreateProgram())
	{
	}

	~ShaderProgram()
	{
		if (id) glDeleteProgram(id);
	}

	void compile(const char* shader_path, GLenum shader_type)
	{
		std::string shader_code;
		std::ifstream shader_file;
		const char* c_shader_code;

		shader_file.open(shader_path);

		if (shader_file.is_open())
		{
			std::stringstream shader_stream;
			shader_stream << shader_file.rdbuf();

			shader_file.close();

			shader_code = shader_stream.str();
			c_shader_code = shader_code.c_str();
		}
		else
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n";
			return;
		}

		GLuint shader_id = glCreateShader(shader_type);

		glShaderSource(shader_id, 1, &c_shader_code, NULL);
		glCompileShader(shader_id);
		checkCompileErrors(shader_id, "SHADER");

		glAttachShader(id, shader_id);
		glLinkProgram(id);
		checkCompileErrors(id, "PROGRAM");

		glDeleteShader(shader_id);
	}

	void use()
	{
		glUseProgram(id);
	}

	void addUniform(const char* name)
	{
		GLuint location = glGetUniformLocation(id, name);
		uniform_locations.try_emplace(name, location);
	}

	void setUniform(const char* name, float value)
	{
		glUniform1f(uniform_locations[name], value);
	}

	void setUniform(const char* name, const glm::vec3& vec)
	{
		glUniform3f(uniform_locations[name], vec.x, vec.y, vec.z);
	}

	void setUniform(const char* name, const float* matrix)
	{
		glUniformMatrix4fv(uniform_locations[name], 1, GL_FALSE, matrix);
	}

private:
	void checkCompileErrors(GLuint shader, const std::string& type)
	{
		int success;
		char infoLog[1024];

		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);

			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}

	GLuint id;
	std::map<std::string, GLuint> uniform_locations;
};