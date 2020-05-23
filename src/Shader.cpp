#include "Shader.hpp"
#include "Renderer.hpp"

#include <GL/glew.h>
#include <iostream>
#include <fstream>

namespace Shader {
	std::string readShader(const int type, std::ifstream* stream) {
		std::string program = "";
		stream->clear();
		stream->seekg(0, std::ios::beg);

		std::string line;
		while (getline(*stream, line)) {
			if (line.find("#version") != std::string::npos) {
				program += line + "\n";

				if (type == GL_VERTEX_SHADER) {
					program += "#define VERTEX_SHADER\n";
				} else if (type == GL_FRAGMENT_SHADER) {
					program += "#define FRAGMENT_SHADER\n";
				}
			} else {
				program += line + "\n";
			}
		}

		return program;
	}

	ShaderPrograms Shader::ParseShader(const std::string& filepath) {
		std::ifstream stream(filepath);

		ShaderPrograms shaders;
		shaders.vertexShader = readShader(GL_VERTEX_SHADER, &stream);
		shaders.fragmentShader = readShader(GL_FRAGMENT_SHADER, &stream);

		stream.close();

		return shaders;
	}

	unsigned Shader::CompileShader(unsigned type, const std::string& source) {
		unsigned id = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

			std::string message;
			message.reserve(length);

			glGetShaderInfoLog(id, length, &length, (GLchar*)message.c_str());

			std::cout << "Failed to compile " <<
				(type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader.\n" <<
				message.c_str() << std::endl;
			std::cin.get();
			exit(-1);
		}

		return id;
	}

	void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
		GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
	}

	void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2) {
		GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
	}

	void Shader::SetUniform3fv(const std::string& name, glm::vec3 values) {
		GLCall(glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(values)));
	}

	void Shader::SetUniform1f(const std::string& name, float value) {
		GLCall(glUniform1f(GetUniformLocation(name), value));
	}

	void Shader::SetUniform1i(const std::string& name, int value) {
		GLCall(glUniform1i(GetUniformLocation(name), value));
	}

	void Shader::SetUniformMat4f(const std::string& name, glm::mat4 matrix) {
		GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
	}

	int Shader::GetUniformLocation(const std::string& name) {
		if (m_LocationCache.find(name) != m_LocationCache.end()) {
			return m_LocationCache[name];
		}

		GLCall(int location = glGetUniformLocation(m_ProgramID, name.c_str()));
		if (location == -1) {
			std::cout << "[Warning] No location with name: " << name << std::endl;
		}
		m_LocationCache[name] = location;

		return location;
	}

	Shader::Shader(const std::string& filePath) {
		ShaderPrograms shaders = Shader::ParseShader(filePath);

		unsigned program = glCreateProgram();
		unsigned vs = Shader::CompileShader(GL_VERTEX_SHADER, shaders.vertexShader);
		unsigned fs = Shader::CompileShader(GL_FRAGMENT_SHADER, shaders.fragmentShader);

		GLCall(glAttachShader(program, vs));
		GLCall(glAttachShader(program, fs));
		GLCall(glLinkProgram(program));
		GLCall(glValidateProgram(program));

		GLCall(glDeleteShader(vs));
		GLCall(glDeleteShader(fs));

		m_ProgramID = program;
	}

	Shader::~Shader() {
		Unbind();
		GLCall(glDeleteProgram(m_ProgramID));
	}

	void Shader::Bind() const {
		GLCall(glUseProgram(m_ProgramID));
	}

	void Shader::Unbind() const {
		GLCall(glUseProgram(0));
	}
}