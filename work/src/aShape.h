#pragma once
#include <glm/glm.hpp>
#include "ray.h"
#include "bbox.h"

struct material {
public:
	glm::vec3 emissionColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	float specularHardness;

	glm::vec3 reflectionColor;
	glm::vec3 refractionColor;
	float ior;
};


struct CollisionInfo {
public:
	float m_distance;
	glm::vec3 m_point;
	glm::vec3 m_normal;

	material m_material;
};


class aShape {
protected:
	glm::vec3 m_center;
	material m_material;
	bbox m_bbox;
public:
	virtual bool intersects(ray r, CollisionInfo& collisionInfo) = 0;
	bbox getBoundingBox() {
		return m_bbox;
	}
};
