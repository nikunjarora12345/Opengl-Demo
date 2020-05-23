#pragma once

#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

namespace Renderer {
	class VertexArray {
	private:
		unsigned m_RendererID;
	public:
		VertexArray();
		~VertexArray();

		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
		void Bind() const;
		void Unbind() const;
	};
}

