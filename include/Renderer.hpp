#pragma once

#include <GL/glew.h>

#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"

// OpenGL Debuggin helper functions
#define GLCall(x) \
	GLClearError(); \
	x; \
	GLLogCall(#x, __FILE__, __LINE__)

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

namespace Renderer {
	class Renderer {
	public:
		Renderer();

		void Clear() const;
		void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader::Shader& shader) const;
		void Draw(const VertexArray& va, const Shader::Shader& shader, unsigned count) const;
	};
}

