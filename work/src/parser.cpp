#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <fstream> 
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "parser.h"
#include "pointLight.h"
#include "sphere.h"
#include "cube.h"
#include "mesh.h"
#include "cylinder.h"

using namespace glm;
using namespace std;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
static const std::string slash = "\\";
#else
static const std::string slash = "/";
#endif

/*******************************************************\
|*******************************************************|
|                    Parsing Functions                  |
|*******************************************************|
\*******************************************************/
string parser::getNextToken(ifstream &stream) {
	string token;
	stream >> token;

	// Ignore comments
	size_t commentPos = token.find('#');
	if (commentPos != string::npos) {
		token = token.substr(0, commentPos);
		string comment;
		getline(stream, comment);
	}
	return token;
}

void parser::parseFilm(std::ifstream &stream) {
	string xresolutionString, yresolutionString, outputFile;
	stream >> xresolutionString >> yresolutionString;
	m_sceneInfo.m_xResolution = stoi(xresolutionString);
	m_sceneInfo.m_yResolution = stoi(yresolutionString);
	m_sceneInfo.m_outputFilename = m_filepath + getNextToken(stream);
}

void parser::parseLookAt(std::ifstream &stream) {
	vec3 position, target, up;
	stream >> position.x >> position.y >> position.z;
	stream >> target.x >> target.y >> target.z;
	stream >> up.x >> up.y >> up.z;
	m_sceneInfo.m_camera->setLookAt(position, target, up);
}

void parser::parseCamera(std::ifstream &stream) {
	std::string fovString;
	fovString = getNextToken(stream);
	m_sceneInfo.m_camera->setFOV(stof(fovString));
}

void parser::parseIntegrator(std::ifstream &stream) {
	string depthString;
	depthString = getNextToken(stream);
	m_sceneInfo.m_maxdepth = stoi(depthString);
}

void parser::parseThreads(std::ifstream &stream) {
	string threadNum = getNextToken(stream);
	m_sceneInfo.m_threads = atoi(threadNum.c_str());
}

/*******************************************************\
|*******************************************************|
|                World Parsing Functions                |
|*******************************************************|
\*******************************************************/
void parser::parseWorld(std::ifstream &stream) {
	string token = "";
	while (stream.good() && token != "WorldEnd") {
		token = getNextToken(stream);
		if (token == "AttributeBegin") {
			parseAttribute(stream);
		}
	}
}

void parser::parseAttribute(ifstream &stream) {
	string token = "";
	vec3 translation(0);
	material mat;
	mat.emissionColor = vec3(0);
	mat.diffuseColor = vec3(1, 1, 0);
	mat.specularColor = vec3(0.5f, 0.5f, 0.5f);
	mat.specularHardness = 5;
	mat.reflectionColor = vec3(0);
	mat.refractionColor = vec3(0);
	mat.ior = 1;

	while (stream.good() && token != "AttributeEnd") {
		token = getNextToken(stream);
		if (token == "Translate") {
			translation += parseTranslate(stream);
		} else if (token == "Shape") {
			parseShape(stream, translation, mat);
		} else if (token == "LightSource") {
			parseLight(stream, translation);
		} else if (token == "Material") {
			mat = parseMaterial(stream);
		}
	}
}

vec3 parser::parseTranslate(ifstream &stream) {
	vec3 translation;
	stream >> translation.x >> translation.y >> translation.z;
	return translation;
}

void parser::parseShape(ifstream &stream, vec3 translation, material mat) {
	string type = getNextToken(stream);
	aShape* shape;

	if (type == "sphere") {
		float radius;
		stream >> radius;
		shape = new sphere(translation, radius, mat);
		m_sceneInfo.shapes.push_back(new sphere(translation, radius, mat));
	} else if (type == "box") {
		vec3 dimensions;
		stream >> dimensions.x >> dimensions.y >> dimensions.z;
		shape = new cube(translation, dimensions.x, dimensions.y, dimensions.z, mat);
	} else if (type == "cylinder") {
		float radius, height;
		stream >> radius >> height;
		m_sceneInfo.shapes.push_back(new cylinder(translation, radius, height, mat));
	} else if (type == "model") {
		string filepath = m_filepath + getNextToken(stream);
		shape = new mesh(filepath, translation, mat);
	}
	else if (type == "trianglemesh") {
		int indexCount = 0;
		stream >> indexCount;
		vector<int> indices;
		for (unsigned int i = 0; i < indexCount; ++i) {
			int index;
			stream >> index;
			indices.push_back(index);
		}

		int vertexCount = 0;
		stream >> vertexCount;
		vector<vec3> vertices;
		for (unsigned int i = 0; i < vertexCount; ++i) {
			vec3 vertex;
			stream >> vertex.x >> vertex.y >> vertex.z;
			vertices.push_back(vertex);
		}

		shape = new mesh(vertices, indices, translation, mat);
	}

	if (shape != nullptr) {
		m_sceneInfo.m_sceneBBox.m_minXYZ = min(m_sceneInfo.m_sceneBBox.m_minXYZ, shape->getBoundingBox().m_minXYZ);
		m_sceneInfo.m_sceneBBox.m_maxXYZ = max(m_sceneInfo.m_sceneBBox.m_maxXYZ, shape->getBoundingBox().m_maxXYZ);
		m_sceneInfo.shapes.push_back(shape);
	}

}

material parser::parseMaterial(ifstream &stream) {
	material mat;
	mat.emissionColor = vec3(0);
	mat.diffuseColor = vec3(0);
	mat.specularColor = vec3(0);
	mat.specularHardness = 0;
	mat.reflectionColor = vec3(-1);
	mat.refractionColor = vec3(-1);
	mat.ior = 1;

	string type = getNextToken(stream);

	if (type == "matte") {
		stream >> mat.diffuseColor.x >> mat.diffuseColor.y >> mat.diffuseColor.z;
	} else if (type == "plastic") {
		stream >> mat.diffuseColor.x >> mat.diffuseColor.y >> mat.diffuseColor.z;
		stream >> mat.specularColor.x >> mat.specularColor.y >> mat.specularColor.z;
		stream >> mat.specularHardness;
		mat.specularHardness = mat.specularHardness;
	} else if (type == "mirror") {
		stream >> mat.reflectionColor.x >> mat.reflectionColor.y >> mat.reflectionColor.z;
	} else if (type == "glass") {
		stream >> mat.reflectionColor.x >> mat.reflectionColor.y >> mat.reflectionColor.z;
		stream >> mat.refractionColor.x >> mat.refractionColor.y >> mat.refractionColor.z;
		stream >> mat.ior;
	} else if (type == "phong") {
		stream >> mat.emissionColor.x >> mat.emissionColor.y >> mat.emissionColor.z;
		stream >> mat.diffuseColor.x >> mat.diffuseColor.y >> mat.diffuseColor.z;
		stream >> mat.specularColor.x >> mat.specularColor.y >> mat.specularColor.z;
		stream >> mat.specularHardness;
		mat.specularHardness = mat.specularHardness;

		stream >> mat.reflectionColor.x >> mat.reflectionColor.y >> mat.reflectionColor.z;

		stream >> mat.refractionColor.x >> mat.refractionColor.y >> mat.refractionColor.z;
		stream >> mat.ior;
	}
	return mat;
}


void parser::parseLight(ifstream &stream, vec3 translation) {
	getNextToken(stream);
	vec3 color;
	stream >> color.x >> color.y >> color.z;
	m_sceneInfo.lights.push_back(new pointLight(color, 1, translation));
}

/*******************************************************\
|*******************************************************|
|                     Constructor                       |
|*******************************************************|
\*******************************************************/
parser::parser(string filename) {
	ifstream pbrtFile(filename);

	if (!pbrtFile.is_open()) {
		cerr << "Error reading " << filename << "\n";
		throw runtime_error("Error :: could not open file.");
	}

	cout << "Reading file " << filename << "\n";
	m_sceneInfo.m_sceneBBox.m_maxXYZ = vec3(-100000);
	m_sceneInfo.m_sceneBBox.m_minXYZ = vec3(100000);

	const size_t last_slash_idx = filename.rfind(slash);
	if (string::npos != last_slash_idx)
	{
		m_filepath = filename.substr(0, last_slash_idx) + slash;
	}

	// good() means that failbit, badbit and eofbit are all not set
	while (pbrtFile.good()) {
		string token = getNextToken(pbrtFile);

		if (token == "Film") {
			parseFilm(pbrtFile);
		} else if (token == "LookAt") {
			parseLookAt(pbrtFile);
		} else if (token == "Camera") {
			parseCamera(pbrtFile);
		} else if (token == "SurfaceIntegrator") {
			parseIntegrator(pbrtFile);
		} else if (token == "Threads") {
			parseThreads(pbrtFile);
		} else if (token == "WorldBegin") {
			parseWorld(pbrtFile);
		} else if (token.size() > 0) {
			if (!pbrtFile.is_open()) {
				cerr << "Error reading " << filename << "\n";
				throw runtime_error("Error :: could not read " + token);
			}
		}
	}
}