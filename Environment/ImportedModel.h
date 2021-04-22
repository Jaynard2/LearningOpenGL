#pragma once
#include <vector>
#include <glm/glm.hpp>

class ImportedModel
{
public:
	ImportedModel(const char* filePath);

	//Getters
	unsigned getNumVertices() const { return _numVertices; }
	std::vector<glm::vec3> getVertices() const { return _vertices; }
	std::vector<glm::vec2> getTexCoords() const { return _texCoords; }
	std::vector<glm::vec3> getNormals() const { return _normals; }

private:
	unsigned _numVertices;
	std::vector<glm::vec3> _vertices;
	std::vector<glm::vec2> _texCoords;
	std::vector<glm::vec3> _normals;

};

