#pragma once
#include <vector>

class ModelImporter
{
public:
	ModelImporter();

	void parseOBJ(const char* filePath);

	//Getters
	unsigned getNumVertices() const { return _triangleVerts.size() / 3; }
	std::vector<float> getVertices() const { return _triangleVerts; }
	std::vector<float> getTexCoords() const { return _textureCoords; }
	std::vector<float> getNormals() const { return _normals; }

private:
	std::vector<float> _vertVals;
	std::vector<float> _stVals;
	std::vector<float> _normVals;

	std::vector<float> _triangleVerts;
	std::vector<float> _textureCoords;
	std::vector<float> _normals;

};

