#pragma once
#include <glm/glm.hpp>

class aLight {
public:
	virtual glm::vec3 getPosition() = 0;
	virtual glm::vec3 calculateLight(glm::vec3 objectPosition, glm::vec3 objectNormal) = 0;
	virtual glm::vec3 getLightColor() = 0;
};
