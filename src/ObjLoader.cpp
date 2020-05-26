#include "ObjLoader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

namespace Loader {
	ObjLoader::ObjLoader(const std::string& filePath) {
		std::vector<unsigned> vertexIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_uvs;
		std::vector<glm::vec3> temp_normals;

		// Read the file
		std::ifstream stream(filePath);

		if (!stream) {
			std::cout << "Error opening file " << filePath << std::endl;
			exit(-1);
		}

		// Extract information line by line
		std::string line;
		while (std::getline(stream, line)) {
			// Vertices
			if (line.substr(0, 2) == "v ") {
				std::istringstream v(line.substr(2));
				float x, y, z;
				v >> x; v >> y; v >> z;
				
				temp_vertices.push_back(glm::vec3(x, y, z));
			}

			// Texture coords
			if (line.substr(0, 2) == "vt") {
				std::istringstream vt(line.substr(2));
				float x, y;
				vt >> x; vt >> y;

				temp_uvs.push_back(glm::vec2(x, y));
			}

			// Normals
			if (line.substr(0, 2) == "vn") {
				std::istringstream v(line.substr(2));
				float x, y, z;
				v >> x; v >> y; v >> z;

				temp_normals.push_back(glm::vec3(x, y, z));
			}

			// Faces
			if (line.substr(0, 2) == "f ") {
				unsigned v_1, v_2, v_3, vt_1, vt_2, vt_3, vn_1, vn_2, vn_3;
				sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
					&v_1, &vt_1, &vn_1, &v_2, &vt_2, &vn_2, &v_3, &vt_3, &vn_3);

				// Decrement all them so as index starts from 0
				v_1--; v_2--; v_3--;
				vt_1--; vt_2--; vt_3--;
				vn_1--; vn_2--; vn_3--;

				vertexIndices.push_back(v_1);
				vertexIndices.push_back(v_2);
				vertexIndices.push_back(v_3);

				uvIndices.push_back(vt_1);
				uvIndices.push_back(vt_2);
				uvIndices.push_back(vt_3);

				normalIndices.push_back(vn_1);
				normalIndices.push_back(vn_2);
				normalIndices.push_back(vn_3);
			}
		}

		for (int i = 0; i < vertexIndices.size(); i++) {
			vertices.push_back(temp_vertices[vertexIndices[i]]);
			uvs.push_back(temp_uvs[uvIndices[i]]);
			normals.push_back(temp_normals[normalIndices[i]]);
		}
	}

	std::vector<float> ObjLoader::GetVertexData() {
		std::vector<float> out(8 * vertices.size());

		for (int i = 0, index = 0; i < 8 * vertices.size(); i += 8, index++) {
			// Vertices
			out[i] = vertices[index].x;
			out[i + 1] = vertices[index].y;
			out[i + 2] = vertices[index].z;

			// UVs
			out[i + 3] = uvs[index].x;
			out[i + 4] = uvs[index].y;

			// Normals
			out[i + 5] = normals[index].x;
			out[i + 6] = normals[index].y;
			out[i + 7] = normals[index].z;
		}

		return out;
	}
}