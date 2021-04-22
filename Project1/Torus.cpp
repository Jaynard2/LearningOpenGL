#include "Torus.h"
#include <glm/gtc/matrix_transform.hpp>

Torus::Torus() : _prec(48), _inner(0.5f), _outer(0.2f)
{
	init();
}

Torus::Torus(float innerRadius, float outerRadius, unsigned prec) :
	_prec(prec), _inner(innerRadius), _outer(outerRadius)
{
	init();
}

void Torus::init()
{
	_numVertices = (_prec + 1) * (_prec + 1);
	_numIndices = _prec * _prec * 6;

	_vertices.resize(_numVertices);
	_texCoords.resize(_numVertices);
	_normals.resize(_numVertices);
	_sTangents.resize(_numVertices);
	_tTangents.resize(_numVertices);

	_indices.resize(_numIndices);

	//calc first ring
	for (unsigned i = 0; i <= _prec; i++)
	{
		float amt = toRadians(i * 360.0f / _prec);

		//build ring by rotating points around origin and moving them outward
		glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 initPos(rMat * glm::vec4(_outer, 0.0f, 0.0f, 1.0f));
		_vertices[i] = glm::vec3(initPos + glm::vec3(_inner, 0.0f, 0.0f));

		//Compute tecoords for each vertex on ring
		_texCoords[i] = glm::vec2(0.0f, (float)i / (float)_prec);

		//compute tangents and normals
		_tTangents[i] = glm::vec3(rMat * glm::vec4(0.0f, -1.0f, 0.0f, 1.0f));
		_sTangents[i] = glm::vec3(glm::vec3(0.0f, 0.0f, -1.0f));
		_normals[i] = glm::cross(_tTangents[i], _sTangents[i]);
	}

	//rotate first ring about y-axis to get other rings
	for (unsigned ring = 1; ring <= _prec; ring++)
	{
		for (unsigned vert = 0; vert <= _prec; vert++)
		{
			//rotate vertex pos of original ring about y-axis
			float amt = toRadians(ring * 360.0f / _prec);
			glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), amt, glm::vec3(0.0f, 1.0f, 0.0f));
			_vertices[ring * (_prec + 1) + vert] = glm::vec3(rMat * glm::vec4(_vertices[vert], 1.0f));

			//compute new tex coords
			_texCoords[ring * (_prec + 1) + vert] = glm::vec2((float)ring * 2.0f / (float)_prec,
				_texCoords[vert].t);
			if (_texCoords[ring * (_prec + 1) + vert].s > 1.0f)
				_texCoords[ring * (_prec + 1) + vert].s -= 1.0f;

			//rotate tangent and bitangent around y-axis
			_sTangents[ring * (_prec + 1) + vert] = glm::vec3(rMat * glm::vec4(_sTangents[vert], 1.0f));
			_tTangents[ring * (_prec + 1) + vert] = glm::vec3(rMat * glm::vec4(_tTangents[vert], 1.0f));

			//rotate normal around y-axis
			_normals[ring * (_prec + 1) + vert] = glm::vec3(rMat * glm::vec4(_normals[vert], 1.0f));
		}
	}

	//calculate triangle indices
	for (unsigned ring = 0; ring < _prec; ring++)
	{
		for (unsigned vert = 0; vert < _prec; vert++)
		{
			_indices[((ring * _prec + vert) * 2) * 3 + 0] = ring * (_prec + 1) + vert;
			_indices[((ring * _prec + vert) * 2) * 3 + 1] = (ring + 1)*(_prec + 1) + vert;
			_indices[((ring * _prec + vert) * 2) * 3 + 2] = ring * (_prec + 1) + vert + 1;
			_indices[((ring * _prec + vert) * 2 + 1) * 3 + 0] = ring * (_prec + 1) + vert + 1;
			_indices[((ring * _prec + vert) * 2 + 1) * 3 + 1] = (ring + 1)*(_prec + 1) + vert;
			_indices[((ring * _prec + vert) * 2 + 1) * 3 + 2] = (ring + 1) * (_prec + 1) + vert + 1;
		}
	}

}

float Torus::toRadians(float degrees) const
{
	return degrees * 3.14159f / 180.0f;
}
