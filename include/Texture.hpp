#pragma once

#include "Renderer.hpp"

namespace Renderer {
	class Texture {
	private:
		unsigned m_RendererID;
		std::string m_FilePath;
		unsigned char* m_LocalBuffer;
		int m_Width, m_Height, m_BitsPerPixel;

	public:
		Texture(const std::string& path, bool freeMemory = true);
		~Texture();

		void Bind(unsigned slot = 0) const;
		void Unbind() const;

		inline int GetWidth() { return m_Width; }
		inline int GetHeight() { return m_Height; }
	};
}
