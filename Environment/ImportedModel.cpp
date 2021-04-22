#include "ImportedModel.h"
#include "ModelImporter.h"

ImportedModel::ImportedModel(const char * filePath)
{
	ModelImporter modelImporter;
	modelImporter.parseOBJ(filePath);

	_numVertices = modelImporter.getNumVertices();
	std::vector<float> verts = modelImporter.getVertices();
	std::vector<float> tcs = modelImporter.getTexCoords();
	std::vector<float> normals = modelImporter.getNormals();

	_vertices.reserve(_numVertices);
	_texCoords.reserve(_numVertices);
	_normals.reserve(_numVertices);

	for (unsigned i = 0; i < _numVertices; i++)
	{
		_vertices.push_back(glm::vec3(verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2]));
		_texCoords.push_back(glm::vec2(tcs[i * 2], tcs[i * 2 + 1]));
		_normals.push_back(glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]));
	}
}
