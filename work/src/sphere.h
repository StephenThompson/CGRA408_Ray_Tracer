#pragma once
#include "glm/glm.hpp"
#include "aShape.h"


class sphere : public aShape {
private:
	float m_radius;
public:
	sphere(glm::vec3 center, float radius, material m);
	bool intersects(ray r, CollisionInfo& collisionInfo);
};
