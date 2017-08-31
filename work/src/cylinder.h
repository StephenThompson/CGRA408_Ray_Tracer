#pragma once
#include "glm/glm.hpp"
#include "aShape.h"


class cylinder : public aShape {
private:
	float m_radius;
	float m_height;
public:
	cylinder(glm::vec3 center, float radius, float height, material m);
	//~sphere();
	bool intersects(ray r, CollisionInfo& collisionInfo);
};
