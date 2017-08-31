#pragma once
#include "glm/glm.hpp"
#include "aLight.h"


class pointLight : public aLight {
private:
	glm::vec3 m_center;
	glm::vec3 m_phi;
	glm::vec3 m_color;
public:
	pointLight(glm::vec3 color, float power, glm::vec3 center);
	glm::vec3 getPosition();
	glm::vec3 calculateLight(glm::vec3 objectPosition, glm::vec3 objectNormal);
	glm::vec3 getLightColor();
};
