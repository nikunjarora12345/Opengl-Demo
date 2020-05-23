#pragma once

#include <vector>
#include <GL/glew.h>

namespace Renderer {
	struct VertexBufferElement {
		unsigned type;
		unsigned count;
		unsigned char normalize;

		unsigned int GetSizeOfType() const {
			switch (type) {
			case GL_FLOAT:
				return sizeof(GLfloat);
			case GL_UNSIGNED_INT:
				return sizeof(GLuint);
			case GL_UNSIGNED_BYTE:
				return sizeof(GLbyte);
			default:
				break;
			}

			return 0;
		}
	};

	class VertexBufferLayout {
	private:
		std::vector<VertexBufferElement> m_Elements;
		unsigned m_Stride;
	public:
		VertexBufferLayout() : m_Stride(0) {}

		template<typename T>
		void Push(unsigned count) {
			static_assert(false);
		}

		template<>
		void Push<float>(unsigned count) {
			VertexBufferElement element = { GL_FLOAT, count, GL_FALSE };

			m_Elements.push_back(element);
			m_Stride += count * element.GetSizeOfType();
		}

		template<>
		void Push<unsigned>(unsigned count) {
			VertexBufferElement element = { GL_UNSIGNED_INT, count, GL_FALSE };

			m_Elements.push_back(element);
			m_Stride += count * element.GetSizeOfType();
		}

		template<>
		void Push<unsigned char>(unsigned count) {
			VertexBufferElement element = { GL_UNSIGNED_BYTE, count, GL_TRUE };

			m_Elements.push_back(element);
			m_Stride += count * element.GetSizeOfType();
		}

		inline const std::vector<VertexBufferElement> GetElements() const& { return m_Elements; }
		inline unsigned GetStride() const { return m_Stride; }
	};
}
