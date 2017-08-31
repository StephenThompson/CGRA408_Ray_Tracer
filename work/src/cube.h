#pragma once
#include "glm/glm.hpp"
#include "aShape.h"


class cube : public aShape {
private:
	glm::vec3 m_minXYZ;
	glm::vec3 m_maxXYZ;
public:
	cube(glm::vec3 center, float width, float height, float length, material m);
	bool intersects(ray r, CollisionInfo& collisionInfo);
};
