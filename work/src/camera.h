#pragma once

#include "glm/glm.hpp"
#include "ray.h"

class camera{
private:
	glm::vec3 m_forward;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_position;

	float m_viewingPlane;
public:
	camera(glm::vec3 position, glm::vec3 center, glm::vec3 up, float fov);
	ray calculateRay(float x, float y);
	void setLookAt(glm::vec3 position, glm::vec3 center, glm::vec3 up);
	void setFOV(float fov);
};