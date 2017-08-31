#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "aShape.h"
#include <algorithm>
#include <limits>

struct bbox {
	glm::vec3 m_minXYZ;
	glm::vec3 m_maxXYZ;
	glm::vec3 center;

	bool intersects(ray r) {
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

		return true;
	}

	/*bool intersects(bbox b) {
		return b.m_minXYZ.x <= m_maxXYZ.x && b.m_maxXYZ.x >= m_minXYZ.x &&
			b.m_minXYZ.y <= m_maxXYZ.y && b.m_maxXYZ.y >= m_minXYZ.y &&
			b.m_minXYZ.z <= m_maxXYZ.z && b.m_maxXYZ.z >= m_minXYZ.z;
	}*/
};