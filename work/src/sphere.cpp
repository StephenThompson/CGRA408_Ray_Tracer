#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "sphere.h"
#include "ray.h"

using namespace glm;

sphere::sphere(vec3 center, float radius, material m) {
	m_center = center;
	m_radius = radius;
	m_material = m;

	m_bbox.center = center;
	m_bbox.m_minXYZ = center - radius;
	m_bbox.m_maxXYZ = center + radius;
}

bool sphere::intersects(ray r, CollisionInfo& collisionInfo) {
	float radiusSq = m_radius * m_radius;
	vec3 x = r.origin - m_center;

	float a = dot(r.direction, r.direction);
	float b = dot(r.direction, x) * 2.0f;
	float c = dot(x, x) - radiusSq;

	float quad = b * b - 4 * a * c;

	if (quad < epsilon<float>())
		return false;

	quad = sqrt(quad);
	float dNear = (-b - quad) / (2 * a);
	float dFar = (-b + quad) / (2 * a);

	if (dNear > dFar) {
		float dTemp = dNear;
		dNear = dFar;
		dFar = dTemp;
	}

	if (dFar < epsilon<float>())
		return false;

	if (dNear > 0) {
		collisionInfo.m_distance = dNear;
		collisionInfo.m_point = r.origin + dNear * r.direction;
	} else {
		collisionInfo.m_distance = dFar;
		collisionInfo.m_point = r.origin + dFar * r.direction;
	}
	collisionInfo.m_normal = normalize(collisionInfo.m_point - m_center);
	collisionInfo.m_material = m_material;
	return true;
}
