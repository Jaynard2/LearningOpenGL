#include "ModelImporter.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

ModelImporter::ModelImporter()
{
}

void ModelImporter::parseOBJ(const char * filePath)
{
	float x, y, z;
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);
	std::string line = "";

	if (fileStream.fail())
	{
		std::cout << "could not find " << filePath << std::endl;
		return;
	}

	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		if (line.compare(0, 2, "v ") == 0)
		{
			std::stringstream ss(line.erase(0, 1));
			ss >> x; ss >> y; ss >> z;
			_vertVals.push_back(x);
			_vertVals.push_back(y);
			_vertVals.push_back(z);
		}
		else if (line.compare(0, 2, "vt") == 0)
		{
			std::stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y;
			_stVals.push_back(x);
			_stVals.push_back(y);
		}
		else if (line.compare(0, 2, "vn") == 0)
		{
			std::stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			_normVals.push_back(x);
			_normVals.push_back(y);
			_normVals.push_back(z);
		}
		else if (line.compare(0, 2, "f ") == 0)
		{
			line.erase(0, 1);
			std::stringstream strss(line);

			for (unsigned i = 0; i < 3; i++)
			{
				std::string cur;
				strss >> cur;

				auto delim1 = cur.find("/");
				auto delim2 = cur.find("/", delim1 + 1);

				std::stringstream vs(cur.substr(0, delim1));
				std::stringstream ts(cur.substr(delim1 + 1, delim2 - delim1 - 1));
				std::stringstream ns(cur.substr(delim2 + 1));

				if (!vs.str().empty())
				{
					unsigned vertRef = (stoi(vs.str()) - 1) * 3;

					_triangleVerts.push_back(_vertVals[vertRef]);
					_triangleVerts.push_back(_vertVals[vertRef + 1]);
					_triangleVerts.push_back(_vertVals[vertRef + 2]);
				}
				if (!ts.str().empty())
				{
					unsigned tcRef = (stoi(ts.str()) - 1) * 2;

					_textureCoords.push_back(_stVals[tcRef]);
					_textureCoords.push_back(_stVals[tcRef + 1]);
				}
				if (!ns.str().empty())
				{
					unsigned normRef = (stoi(ns.str()) - 1) * 3;

					_normals.push_back(_normVals[normRef]);
					_normals.push_back(_normVals[normRef + 1]);
					_normals.push_back(_normVals[normRef + 2]);
				}
			}
		}
	}
}
