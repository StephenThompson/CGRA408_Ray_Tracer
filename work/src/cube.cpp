#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "cube.h"
#include "ray.h"

using namespace glm;

cube::cube(vec3 center, float width, float height, float length, material m) {
	m_center = center;
	m_minXYZ = m_center - vec3(width / 2.f, height / 2.f, length / 2.f);
	m_maxXYZ = m_center + vec3(width / 2.f, height / 2.f, length / 2.f);
	m_material = m;

	m_bbox.center = center;
	m_bbox.m_minXYZ = m_minXYZ;
	m_bbox.m_maxXYZ = m_maxXYZ;
}

bool cube::intersects(ray r, CollisionInfo& collisionInfo) {
	// formula taken from https://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
	float tNear = std::numeric_limits<float>::min();
	float tFar = std::numeric_limits<float>::max();

	// X plane
	if (abs(r.direction.x) < glm::epsilon<float>() && (r.origin.x < m_minXYZ.x || r.origin.x > m_maxXYZ.x)) return false;
	float t1 = (m_minXYZ.x - r.origin.x) / r.direction.x;
	float t2 = (m_maxXYZ.x - r.origin.x) / r.direction.x;

	if (t1 > t2) std::swap(t1, t2);
	if (t1 > tNear) tNear = t1;
	if (t2 < tFar) tFar = t2;
	if (tNear > tFar || tFar < 0) return false;

	// Y plane
	if (abs(r.direction.y) < glm::epsilon<float>() && (r.origin.y < m_minXYZ.y || r.origin.y > m_maxXYZ.y)) return false;
	t1 = (m_minXYZ.y - r.origin.y) / r.direction.y;
	t2 = (m_maxXYZ.y - r.origin.y) / r.direction.y;

	if (t1 > t2) std::swap(t1, t2);
	if (t1 > tNear) tNear = t1;
	if (t2 < tFar) tFar = t2;
	if (tNear > tFar || tFar < 0) return false;

	// Z plane
	if (abs(r.direction.z) < glm::epsilon<float>() && (r.origin.z < m_minXYZ.z || r.origin.z > m_maxXYZ.z)) return false;
	t1 = (m_minXYZ.z - r.origin.z) / r.direction.z;
	t2 = (m_maxXYZ.z - r.origin.z) / r.direction.z;

	if (t1 > t2) std::swap(t1, t2);
	if (t1 > tNear) tNear = t1;
	if (t2 < tFar) tFar = t2;
	if (tNear > tFar || tFar < 0) return false;

	collisionInfo.m_distance = tNear > 0? tNear : tFar;
	collisionInfo.m_point = r.origin + collisionInfo.m_distance * r.direction;
	
	vec3 colScaled = abs((collisionInfo.m_point - m_center) / vec3(m_bbox.m_minXYZ - m_center));
	if (colScaled.x > colScaled.y && colScaled.x > colScaled.z)
	  collisionInfo.m_normal = vec3((m_center.x - collisionInfo.m_point.x) > 0? -1 : 1, 0, 0);
	else if (colScaled.y > colScaled.x && colScaled.y > colScaled.z)
	  collisionInfo.m_normal = vec3(0, (m_center.y - collisionInfo.m_point.y) > 0? -1 : 1, 0);
	else
	  collisionInfo.m_normal = vec3(0, 0, (m_center.z - collisionInfo.m_point.z) > 0? -1 : 1);
	
	collisionInfo.m_material = m_material;
	return true;
}
