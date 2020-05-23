#include "VertexArray.hpp"
#include "Renderer.hpp"

namespace Renderer {
	VertexArray::VertexArray() {
		GLCall(glGenVertexArrays(1, &m_RendererID));
	}

	VertexArray::~VertexArray() {
		Unbind();
		GLCall(glDeleteVertexArrays(1, &m_RendererID));
	}

	void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
		Bind();
		vb.Bind();

		unsigned offset = 0;
		const std::vector<VertexBufferElement>& elements = layout.GetElements();
		for (unsigned i = 0; i < elements.size(); i++) {
			const VertexBufferElement& element = elements[i];
			GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalize,
				layout.GetStride(), (const void*)offset));
			GLCall(glEnableVertexAttribArray(i));
			offset += element.count * element.GetSizeOfType();
		}
	}

	void VertexArray::Bind() const {
		GLCall(glBindVertexArray(m_RendererID));
	}
	
	void VertexArray::Unbind() const {
		GLCall(glBindVertexArray(0));
	}
}