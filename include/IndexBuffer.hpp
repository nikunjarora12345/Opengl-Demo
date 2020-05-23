#pragma once

namespace Renderer {
	class IndexBuffer {
	private:
		unsigned m_RendererID;
		unsigned m_Count;

	public:
		IndexBuffer(const unsigned* data, unsigned count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		inline unsigned GetCount() const { return m_Count; }
	};
};

