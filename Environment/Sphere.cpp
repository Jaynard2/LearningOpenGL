#include "Sphere.h"

Sphere::Sphere()
{
	init(48);
}

Sphere::Sphere(unsigned prec)
{
	init(prec);
}

void Sphere::init(unsigned prec)
{
	_numVertices = (prec + 1) * (prec + 1);
	_numIndices = prec * prec * 6;

	_vertices.resize(_numVertices);
	_texCoords.resize(_numVertices);
	_normals.resize(_numVertices);

	_indices.resize(_numIndices, 0);

	//Calculate vertices
	for (unsigned i = 0; i <= prec; i++)
	{
		for (unsigned j = 0; j <= prec; j++)
		{
			float y = (float)cos(toRadians(180.0f - i * 180.0f / prec));
			float x = -(float)cos(toRadians(j * 360.0f / prec)) * (float)abs(cos(asin(y)));
			float z = (float)sin(toRadians(j * 360.0f / prec)) * (float)abs(cos(asin(y)));

			_vertices[i * (prec + 1) + j] = glm::vec3(x, y, z);
			_texCoords[i * (prec + 1) + j] = glm::vec2((float)j / prec, (float)i / prec);
			_normals[i * (prec + 1) + j] = glm::vec3(x, y, z);
		}
	}

	//Calculate Triangle Indices
	for (unsigned i = 0; i < prec; i++)
	{
		for (unsigned j = 0; j < prec; j++)
		{
			_indices[6 * (i * prec + j) + 0] = i * (prec + 1) + j;
			_indices[6 * (i * prec + j) + 1] = i * (prec + 1) + j + 1;
			_indices[6 * (i * prec + j) + 2] = (i + 1) * (prec + 1) + j;
			_indices[6 * (i * prec + j) + 3] = i * (prec + 1) + j + 1;
			_indices[6 * (i * prec + j) + 4] = (i + 1) * (prec + 1) + j + 1;
			_indices[6 * (i * prec + j) + 5] = (i + 1) * (prec + 1) + j;
		}
	}

}

float Sphere::toRadians(float degrees) const
{
	return degrees * 3.14159f / 180.0f;
}
