#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "triangle.h"
#include "ray.h"

using namespace glm;

triangle::triangle(vec3 p0, vec3 p1, vec3 p2, vec3 n0, vec3 n1, vec3 n2, material m) {
	m_p0 = p0;
	m_p1 = p1;
	m_p2 = p2;

	m_n0 = n0;
	m_n1 = n1;
	m_n2 = n2;
	m_flatNormal = normalize(cross(m_p0 - m_p1, m_p0 - m_p2));

	m_bbox.m_minXYZ = min(min(p0, p1), p2);
	m_bbox.m_maxXYZ = max(max(p0, p1), p2);
	m_bbox.center = (m_bbox.m_minXYZ + m_bbox.m_maxXYZ) * 0.5f;

	m_material = m;
}

bool triangle::intersects(ray r, CollisionInfo& collisionInfo) {
	vec3 e1 = m_p1 - m_p0, e2 = m_p2 - m_p0, s = r.origin - m_p0;

	float det = 1.f/dot(cross(r.direction, e2), e1);
	float t = det * dot(cross(s, e1), e2);
	float b0 = det * dot(cross(r.direction, e2), s);
	float b1 = det * dot(cross(s, e1), r.direction);

	float ep = epsilon<float>() * 1000;
	if (b0 < -ep || b1 < -ep || b0 + b1 > 1 + ep || t < 0)
		return false;

	collisionInfo.m_distance = t;
	collisionInfo.m_point = r.origin + t * r.direction;
	collisionInfo.m_normal = normalize(m_n0 * (1.f - b0 - b1) + m_n1 * b0 + m_n2 * b1);
	collisionInfo.m_material = m_material;
	return true;
}
