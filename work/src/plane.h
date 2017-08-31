#pragma once
#include "glm/glm.hpp"
#include "aShape.h"


class plane : public aShape {
private:
	glm::vec3 m_normal;
	float m_amount;
public:
	plane(glm::vec3 normal, float amount, material m);
	//~sphere();
	bool intersects(ray r, CollisionInfo& collisionInfo);
};
