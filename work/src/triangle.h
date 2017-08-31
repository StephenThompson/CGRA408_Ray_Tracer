#pragma once
#include "glm/glm.hpp"
#include "aShape.h"


class triangle : public aShape {
private:
	glm::vec3 m_p0;
	glm::vec3 m_p1;
	glm::vec3 m_p2;
	glm::vec3 m_n0;
	glm::vec3 m_n1;
	glm::vec3 m_n2;
	glm::vec3 m_flatNormal;
public:
	triangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 n0, glm::vec3 n1, glm::vec3 n2, material m);
	bool intersects(ray r, CollisionInfo& collisionInfo);
};
