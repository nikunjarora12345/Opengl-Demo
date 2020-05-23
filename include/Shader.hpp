#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Shader {
	struct ShaderPrograms {
		std::string vertexShader;
		std::string fragmentShader;
	};

	class Shader {
	private:
		unsigned m_ProgramID;
		std::unordered_map<std::string, int> m_LocationCache; // For Caching Uniform Locations

		int GetUniformLocation(const std::string& name);
	public:
		Shader(const std::string& filepath);
		~Shader();

		void Bind() const;
		void Unbind() const;

		static ShaderPrograms ParseShader(const std::string&);
		static unsigned CompileShader(unsigned, const std::string&);

		// Set Uniforms
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniform3f(const std::string& name, float v0, float v1, float v2);
		void SetUniform3fv(const std::string& name, glm::vec3 values);
		void SetUniform1f(const std::string& name, float value);
		void SetUniform1i(const std::string& name, int value);
		void SetUniformMat4f(const std::string& name, glm::mat4 matrix);
	};
}