#pragma once
#include <string>
#include <fstream> 
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <iostream>
#include <string>
#include <vector>
#include "glm/glm.hpp"

#include "aShape.h"
#include "aLight.h"
#include "camera.h"


struct sceneInformation {
	// Rendering data
	std::string m_outputFilename = "img.png";
	int m_xResolution = 640, m_yResolution = 480, m_maxdepth = 4, m_threads = 1;

	// Camera data
	camera* m_camera = new camera(glm::vec3(0, 2, 0), glm::vec3(), glm::vec3(0, 1, 0), 90);

	// Shape data
	std::vector<aShape*> shapes;
	bbox m_sceneBBox;

	// Light data
	std::vector<aLight*> lights;
};

class parser{
private:
	std::string getNextToken(std::ifstream &stream);

	void parseLookAt(std::ifstream &stream);
	void parseCamera(std::ifstream &stream);
	void parseFilm(std::ifstream &stream);
	void parseIntegrator(std::ifstream &stream);
	void parseThreads(std::ifstream &stream);

	void parseWorld(std::ifstream &stream);
	void parseAttribute(std::ifstream &stream);
	glm::vec3 parseTranslate(std::ifstream &stream);
	material parseMaterial(std::ifstream &stream);
	void parseShape(std::ifstream &stream, glm::vec3 translation, material mat);
	void parseLight(std::ifstream &stream, glm::vec3 translation);

	sceneInformation m_sceneInfo;
	std::string m_filepath;

public:
	parser(std::string filename);
	sceneInformation getSceneInfo() {
		return m_sceneInfo;
	}
};