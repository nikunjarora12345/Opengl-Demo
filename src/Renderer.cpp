#include <iostream>

#include "Renderer.hpp"

void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
	if (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << error << "): "
			<< function << " " << file << ":" << line << std::endl;
		return false;
	}

	return true;
}

namespace Renderer {
	Renderer::Renderer() {
		// Setup OpenGL
		
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glDepthFunc(GL_LESS));

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	}

	void Renderer::Clear() const {
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader::Shader& shader) const {
		va.Bind();
		ib.Bind();
		shader.Bind();

		GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
	}

	void Renderer::Draw(const VertexArray& va, const Shader::Shader& shader, unsigned count) const {
		va.Bind();
		shader.Bind();

		GLCall(glDrawArrays(GL_TRIANGLES, 0, count));
	}
}
