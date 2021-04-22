#pragma once
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

class Sphere
{
public:
	Sphere();
	Sphere(unsigned prec);

	//Getters
	unsigned getNumVertices() const { return _numVertices; }
	unsigned getNumIndices() const { return _numIndices; }
	std::vector<unsigned> getIndices() const { return _indices; }
	std::vector<glm::vec3> getVertices() const { return _vertices; }
	std::vector<glm::vec2> getTexCoords() const { return _texCoords; }
	std::vector<glm::vec3> getNormals() const { return _normals; }

private:
	unsigned _numVertices;
	unsigned _numIndices;
	std::vector<unsigned> _indices;
	std::vector<glm::vec3> _vertices;
	std::vector<glm::vec2> _texCoords;
	std::vector<glm::vec3> _normals;

	void init(unsigned prec);
	float toRadians(float degrees) const;

};

