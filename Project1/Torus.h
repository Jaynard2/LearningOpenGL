#pragma once
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

class Torus
{
public:
	Torus();
	Torus(float innerRadius, float outerRadius, unsigned prec);

	//Getters
	unsigned getNumVertices() const { return _numVertices; }
	unsigned getNumIndices() const { return _numIndices; }

	std::vector<unsigned> const getIndices() { return _indices; }
	std::vector<glm::vec3> getVertices() const { return _vertices; }
	std::vector<glm::vec2> getTexCoords() const { return _texCoords; }
	std::vector<glm::vec3> getNormals() const { return _normals; }
	std::vector<glm::vec3> getSTangets() const { return _sTangents; }
	std::vector<glm::vec3> getTTangets() const { return _tTangents; }

private:
	unsigned _numVertices;
	unsigned _numIndices;
	unsigned _prec;
	float _inner;
	float _outer;

	std::vector<unsigned>  _indices;
	std::vector<glm::vec3> _vertices;
	std::vector<glm::vec2> _texCoords;
	std::vector<glm::vec3> _normals;
	std::vector<glm::vec3> _sTangents;
	std::vector<glm::vec3> _tTangents;

	void init();
	float toRadians(float degrees) const;

};

