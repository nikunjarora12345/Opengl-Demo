#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>

namespace Loader {
	class ObjLoader {
	public:
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;

		ObjLoader(const std::string& filePath);

		std::vector<float> GetVertexData();
	};
}

