#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "plane.h"
#include "ray.h"

using namespace glm;

plane::plane(vec3 normal, float height, material m) {
	m_normal = normal;
	m_amount = height;
	m_material = m;
}


bool plane::intersects(ray r, CollisionInfo& collisionInfo) {
	float nDotr = dot(r.direction, m_normal);

	if (abs(nDotr) < epsilon<float>() * 10000) return false;

	float distance = (m_amount - dot(m_normal, r.origin)) / nDotr;

	collisionInfo.m_distance = distance;
	collisionInfo.m_point = r.origin + distance * r.direction;
	collisionInfo.m_normal = m_normal;
	collisionInfo.m_material = m_material;
	return true;
}
